// The Master Control Program is the Server that NekotOS cocos connect to.
// At the moment, this is dispatched from the Lemma Server in frobio,
// upon it receiving a quint with `CMD_HELLO_NEKOTOS = 64`.
package mcp

import (
	"bytes"
	"flag"
	"log"
	"net"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/strickyak/nekotos/data"
	"github.com/strickyak/nekotos/mcp/transcript"
	. "github.com/strickyak/nekotos/mcp/util"
)

var GAMES_DIR = flag.String("games_dir", "/tmp", "where .games files are located")
var GAME_ZONE = flag.String("zone", "America/New_York", "linux time zone location")

const (
	N_CLOSED   = 63
	N_GREETING = 64
	N_MEMCPY   = 65
	N_POKE     = 66
	N_START    = 68
	N_KEYSCAN  = 69
	N_CONTROL  = 70
	CMD_LOG    = 200
)

const (
	// Borrow $FFxx "addresses" for tagging Info
	HELLO_HANDLE  = 0xFF00
	HELLO_NAME    = 0xFF01
	HELLO_ZONE    = 0xFF02
	HELLO_AIRPORT = 0xFF03
)

type Gamer struct {
	Conn      net.Conn
	SendMutex sync.Mutex
	Hellos    map[uint][]byte

	Handle   string
	Name     string
	Zone     string
	Airport  string
	Location *time.Location
	Special  string // special word sent with HELLO(0)
	Room     *Room

	// game  *Game
	// shard *Shard
	// round uint
	// load  *Load

	// line *LineBuf

	Keys     [8]byte
	Line     []byte
	Trans    *transcript.Rec
	ChatPage int // 0 for normal bottom.  Positive from top.

	// Console [14][32]byte

	Level       uint   // Original GREETING `p` parameter
	Greeting    []byte // Original GREETING payload
	NekotOSHash []byte
	ConsAddr    uint
	MaxPlayers  uint
	GWall       uint
	GScore      uint
}

func (o Gamer) String() string {
	return o.Handle
}

type Game struct {
	Name   string
	source string
}

const (
	// InputPacket states
	EMPTY = 0
	HEAD  = 1
	FULL  = 2
)

// MkWord turns a 2-byte slice into BigEndian 2-byte word for Coco.
func MkWord(b []byte) uint {
	hi := uint(b[0])
	lo := uint(b[1])
	return (hi << 8) | lo
}

// Packet has a 5 byte header (1B c, 2B n, 2B p) and arbitrary payload.
type Packet struct {
	c   byte
	n   uint
	p   uint
	pay []byte
}
type InputPacketizer struct {
	Conn  net.Conn
	out   chan<- Packet
	gamer *Gamer
}

func (o *InputPacketizer) Go() {
	defer func() {
		r := recover()
		if r != nil {
			log.Printf("PANIC: user %q error %v", o.gamer.Handle, r)
			close(o.out)
			o.Conn.Close()
		}
	}()

	log.Printf("InputPacketizer GO...")
	for {
		// Get Header
		var header [5]byte

		n, err := o.Conn.Read(header[:])
		if err != nil {
			log.Printf("InputPacketizer %q cannot Read header: %v", o.gamer, err)
			break
		}
		if n != 5 {
			log.Printf("InputPacketizer %q got %d bytes, wanted 5 byte header", o.gamer, n)
			break
		}
		log.Printf("Input: %q got header % 3x", o.gamer, header[:])

		p := Packet{header[0], MkWord(header[1:3]), MkWord(header[3:5]), nil}
		if p.n > 256 {
			log.Panicf("InputPacketizer %q packet too big: % 3x", o.gamer, header[:])
		}
		p.pay = make([]byte, p.n)

		n, err = o.Conn.Read(p.pay)
		if err != nil {
			log.Printf("InputPacketizer %q cannot Read %d byte payload: %v", o.gamer, n, err)
			break
		}
		if uint(n) != p.n {
			log.Printf("InputPacketizer %q got %d bytes, wanted %d byte payload", o.gamer, n, p.n)
			break
		}
		log.Printf("    %q Payload % 3x", o.gamer, p.pay)
		o.out <- p
	}
	o.out <- Packet{0, 0, 0, nil} // End Sentinel
	close(o.out)
}

func (g *Gamer) SendPacket(c byte, p uint, bb []byte) {
	n := uint(len(bb))
	log.Printf("mcp Gamer.SendPacket c=$%x=%d. p=$%x=%d. len=$%x=%d. ( % 3x )", c, c, p, p, n, n, bb)

	buf := []byte{c, byte(n >> 8), byte(n), byte(p >> 8), byte(p)}
	buf = append(buf, bb...)

	g.SendMutex.Lock()
	_ = Value(g.Conn.Write(buf))
	g.SendMutex.Unlock()
}

func (g *Gamer) SendMemCopy(d uint, s uint, count uint) {
	pay := []byte{
		byte(d >> 8),
		byte(d),
		byte(s >> 8),
		byte(s),
		byte(count >> 8),
		byte(count),
	}
	g.SendPacket(N_MEMCPY, 0, pay)
}

func (g *Gamer) SendPokeMemory(p uint, bb []byte) {
	const M = 50
	for len(bb) > M {
		g.SendPacket(N_POKE, p, bb[:M])
		bb = bb[M:]
		p += M
	}
	if len(bb) > 0 {
		g.SendPacket(N_POKE, p, bb)
	}
}

func (g *Gamer) WaitOnPacket(inchan chan Packet) (p Packet, ok bool) {
	p, ok = <-inchan
	if !ok {
		log.Panicf("PollInput: inchan closed")
	}
	return
}

func (g *Gamer) HandlePackets(inchan chan Packet) {
	for {
		if p, ok := g.WaitOnPacket(inchan); ok {
			switch p.c {
			case CMD_LOG:
				Log("N1LOG: %q %q", g.Handle, p.pay)
			case N_KEYSCAN:
				g.KeyScanHandler(p.pay)
			case N_CONTROL:
				g.ControlRequestHandler(p.p, p.pay)
			case N_GREETING:
				Log("%q GREETING(%d) [%d] % 3x", g, p.p, len(p.pay), p.pay)

				if p.p == 1 && len(p.pay) == 16 && string(p.pay[:7]) == "nekotos" {

					g.ConsAddr = WordFromBytes(p.pay, 8)
					g.MaxPlayers = WordFromBytes(p.pay, 10)
					g.GScore = WordFromBytes(p.pay, 12)
					g.GWall = WordFromBytes(p.pay, 14)
					now := g.SendWallTime()
					log.Printf("GREETING(1) sent Wall Time: %v", now)

					g.PrintPlain("******************************")
					g.PrintPlain(Format("*** HANDLE = %q", g.Handle))
					g.PrintPlain(Format("*** NAME = %q", g.Name))
					g.PrintPlain(Format("*** ZONE = %q", g.Zone))
					g.PrintPlain(Format("*** AIRPORT = %q", g.Airport))
					g.PrintPlain(Format("*** (CMSW) = %x %x %x %x", g.ConsAddr, g.MaxPlayers, g.GScore, g.GWall))
					g.PrintPlain(Format("*** (CARD) = %q", g.Special))

					g.ConsoleSync()

				} else if p.p == 2 && len(p.pay) == 8 {

					g.NekotOSHash = p.pay
					log.Printf("GREETING(2) from NekotOSHash % 3x", p.pay)

					g.PrintPlain(Format("*** (HASH) = %x", p.pay[:8]))
					g.ConsoleSync()

				} else {
					log.Panicf("unknown GREETING(%d): % 3x", p.p, p.pay)
				}
			default:
				Log("WUT? default PPI: %v", p)
			}
		} else {
			break
		}
	}
}

func ExtractCString(bb []byte) string {
	var z []byte
	for i := 0; i < 64 && i < len(bb); i++ {
		x := bb[i]
		if x <= 32 || x > 126 {
			return string(z)
		}
		z = append(z, x)
	}
	return string(z)
}

const Dont_CausesHangs = false

func (g *Gamer) ControlRequestHandler(p uint, pay []byte) {
	switch p {
	case 'a': // Game Abort
		why := ExtractCString(pay)
		Log("%q Game Aborted: %q\n", g, why)
		if Dont_CausesHangs {
			g.PrintLine(Format("*** GAME ABORTED: %s", why))
		}
		g.SendPacket(N_START, 0, nil)

	case 'c': // Game Chain
		filename := ExtractCString(pay)
		Log("%q CHAIN => %q <=\n", g, filename)
		if Dont_CausesHangs {
			g.PrintLine(Format("*** GAME CHAIN TO %s", filename))
		}

		// Do not g.SendPacket(N_START, 0, nil),
		// because g.SendGameAndLaunch will do that.
		decb := g.ReadVersionedGameFile(filename)
		g.SendGameAndLaunch(decb)

	case 'o': // Game Over
		why := ExtractCString(pay)
		Log("%q Game Over: %q\n", g, why)
		if Dont_CausesHangs {
			g.PrintLine(Format("*** GAME OVER: %s", why))
		}
		g.SendPacket(N_START, 0, nil)

	case 'L': // Logging
		Log("N1Log: %q logs %q", g.Handle, pay)

	case 'S': // Partial Scoring
		// Handle Partial Scores
		Log("N1: %q scores % 3x", g.Handle, pay)

	default:
		panic(p)
	}
}

const (
	UP    = 0o013 // UP is Vertical Tab = 11.
	DOWN  = 0o014 // DOWN is Form Feed = 12.
	LEFT  = 0o010 // LEFT is Backspace = 8.
	RIGHT = 0o011 // RIGHT is Tab = 9.
	CLEAR = 0o007 // CLEAR is Bell = 7
	ENTER = 0o012 // ENTER is Line Feed (\n) = 10
)

var CharLookup = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ\013\014\010\011 0123456789:;,-./\012\007"

func WhatChar(i int, y byte) byte {
	index := 999
	for j := 0; j < 8; j++ {
		if (y & (byte(1) << j)) != 0 {
			index = i + j*8
			break
		}
	}
	if index < len(CharLookup) {
		return CharLookup[index]
	} else {
		return 0
	}
}

func (g *Gamer) KeyScanHandler(bb []byte) {
	var ch byte
	for i, x := range bb {
		prev := g.Keys[i]
		if prev != x {
			// This byte changed.
			y := (x ^ prev) & x // bits added.
			ch = WhatChar(i, y)
			if ch != 0 {
				printable := ""
				if 32 <= ch && ch <= 126 {
					printable = string([]byte{ch})
				}
				log.Printf("KeyScanHandler: ch=%d(%q) y=%x prev=%x i=%x bb=(%02x) keys=(%02x)", ch, printable, y, prev, i, bb, g.Keys[:])
				break
			}
		}
	}

	copy(g.Keys[:], bb) // Save keyscan.
	if ch != 0 {
		g.InkeyHandler(ch)
	}
}

const (
	C_CLEAR = 7
	C_LEFT  = 8
	C_RIGHT = 9
	C_ENTER = 10
	C_UP    = 11
	C_DOWN  = 12
)

func (g *Gamer) PageUp() {
	if g.ChatPage == 0 {
		g.ChatPage = g.Trans.NumPages()
	} else if g.ChatPage > 1 {
		g.ChatPage--
	}
	g.ConsoleSync()
}

func (g *Gamer) PageDown() {
	if g.ChatPage == g.Trans.NumPages() {
		g.ChatPage = 0 // the "very bottom"
	} else if g.ChatPage == 0 {
		g.ChatPage = 0 // already at "the very bottom"
	} else {
		g.ChatPage++
	}
	g.ConsoleSync()
}

func (g *Gamer) InkeyHandler(ch byte) {
	needsSync := false
	if ' ' <= ch && ch <= '~' {
		if g.ChatPage != 0 {
			g.ChatPage = 0 // jump to bottom
			needsSync = true
		}
		g.Line = append(g.Line, ch)
	} else if ch == C_UP {
		g.PageUp()
	} else if ch == C_DOWN {
		g.PageDown()
	} else if ch == C_LEFT {
		// backspace
		if len(g.Line) > 0 {
			g.Line = g.Line[:len(g.Line)-1] // remove last char
		}
	} else if ch == C_ENTER {
		// execute
		g.EnterLine()
		g.Line = nil
	}
	g.DrawBottomLine()
	if needsSync {
		g.ConsoleSync()
	}
}

func AtoU(s string) uint {
	return uint(Value(strconv.ParseUint(s, 10, 64)))
}

func (g *Gamer) ExecuteSlashCommand(s string) {
	defer func() {
		r := recover()
		if r != nil {
			KernelSendChatf("ERROR: %v", r)
		}
	}()
	ww := strings.Split(s, " ")
	n := len(ww)
	cmd := ww[0]

	switch {
	case strings.HasPrefix(cmd, "J"):
		switch n {
		case 1:
			CommandJoin(g, 0)
		case 2:
			CommandJoin(g, AtoU(ww[1]))
		default:
			panic("too many args")
		}
	case strings.HasPrefix(cmd, "W"):
		CommandWho(g)
	default:
		g.Printf("Unknown slash command: %q")
	}
}

func (g *Gamer) ReadVersionedGameFile(gamename string) []byte {
	// clean the gamename: remove Dir and .game extension, and lowercase
	basename := strings.ToLower(strings.TrimSuffix(filepath.Base(gamename), ".game"))
	// want correct version of game, in the GAMES_DIR
	filename := filepath.Join(*GAMES_DIR, Format("%s.%02x.game", basename, g.NekotOSHash))
	log.Printf("%v ReadVersionedGameFile: %q", g, filename)
	// slurp and return contents
	contents, err := os.ReadFile(filename)
	if err != nil {
		return nil
	}
	return contents
}

func (g *Gamer) Printf(format string, args ...any) {
	g.PrintPlain(Format(format, args...))
}

func (g *Gamer) LaunchGame(gamename string) {
	decb := g.ReadVersionedGameFile(gamename)
	if decb == nil {
		g.Printf("Game not found: %q", gamename)
		return
	}
	g.SendGameAndLaunch(decb)
}

func (g *Gamer) EnterLine() {
	s := string(g.Line)
	log.Printf("GAMER %q LINE %q\n", g.Handle, s)

	GamerSendChat(g, s)

	if strings.HasPrefix(s, "/") {
		g.ExecuteSlashCommand(s[1:])
	} else if s == "0" {
		log.Printf("-> STOP <-")
		g.SendPacket(N_START, 0, nil)
	} else if s == "C" {
		g.LaunchGame("clock")
	} else if s == "R" {
		g.LaunchGame("red")
	} else if s == "G" {
		g.LaunchGame("green")
	} else if s == "B" {
		g.LaunchGame("blue")
	} else if s == "L" {
		g.LaunchGame("life")
	} else if s == "8" {
		g.LaunchGame("lib8")
	} else if s == "4" {
		g.LaunchGame("forth")
	} else if s == "S" {
		g.LaunchGame("spacewar")
	}
}

func (g *Gamer) DrawBottomLine() {
	const BlueBottom = 0xA3
	const RedBox = 0xFF
	var bar [32]byte
	n := len(g.Line)
	for i := 0; i < 32; i++ {
		if 1 <= i && i <= n {
			bar[i] = 63 & g.Line[i-1]
		} else if i == n+1 {
			bar[i] = RedBox
		} else {
			bar[i] = BlueBottom
		}
	}
	g.SendPokeMemory(0x05E0, bar[:])
}

func (g *Gamer) ConsoleSync() {
	scr := g.Trans.ScreenPage(g.ChatPage)
	n := len(scr)
	w := len(scr[0])

	for i := 0; i < n; i++ {
		g.SendPokeMemory(0x0420+uint(i*w), scr[i][:])
	}
}

func (g *Gamer) PrintPlain(s string) {
	g.PrintLine(transcript.PlainString(s))
}
func (g *Gamer) PrintLine(s string) {
	g.Trans.AppendString(s)
	g.ConsoleSync()
}

func (gamer *Gamer) Step(inchan chan Packet) {
	gamer.HandlePackets(inchan) // doesn't return until Error
}

// SendGameAndLaunch takes the contents of a DECB binary,
// and pokes it into the Coco.
func (gamer *Gamer) SendGameAndLaunch(bb []byte) {
	// Flip back to Shell mode, so you're not executing the old game
	// while loading the new game.
	gamer.SendPacket(N_START, 0, nil)
	gamer.SendInitializedScores()
	gamer.SendWallTime()

	for len(bb) >= 5 {
		c := bb[0]
		n := (uint(bb[1]) << 8) | uint(bb[2])
		p := (uint(bb[3]) << 8) | uint(bb[4])
		log.Printf("SendGameAndLaunch: %x %x %x (len=%d)", c, n, p, len(bb))
		bb = bb[5:]

		switch c {
		case 0:
			gamer.SendPokeMemory(p, bb[:n])
			bb = bb[n:]

		case 255:
			gamer.SendPacket(N_START, p, nil)

		default:
			panic(bb[0])
		}
	}
}

var Weekdays = []string{"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"}
var Months = []string{"ZZZ", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"}
var ZeroHours = Value(time.ParseDuration("0h"))
var TwentyFourHours = Value(time.ParseDuration("24h"))

func (gamer *Gamer) WallTimeBytes(now time.Time, addMe time.Duration) []byte {
	then := now.Add(addMe)
	y, m, d := then.Date()
	hr, minute, sec := then.Hour(), then.Minute(), then.Second()
	weekday := Weekdays[then.Weekday()]
	month := Months[m]
	wall := []byte{
		byte(sec), byte(minute), byte(hr),
		byte(d), byte(m), byte(y - 2000),
		weekday[0], weekday[1], weekday[2], 0,
		month[0], month[1], month[2], 0,
	}
	return wall
}
func (gamer *Gamer) SendWallTime() time.Time {
	location, err := time.LoadLocation(gamer.Zone)
	if err != nil {
		location = time.UTC
		log.Printf("BAD GAME TIME ZONE %q -- USING UTC INSTEAD", gamer.Zone)
	}
	now := time.Now().In(location)

	bb := gamer.WallTimeBytes(now, ZeroHours)
	bb = append(bb, gamer.WallTimeBytes(now, TwentyFourHours)[3:]...) // omit tomorrow's h, m, s
	log.Printf("SendWallTime $%04x: len=%d % 3x", gamer.GWall, len(bb), bb)
	if gamer.GWall > 0 {
		gamer.SendPacket(N_POKE, gamer.GWall, bb)
	}
	return now
}

func (gamer *Gamer) SendInitializedScores() {
	bb := []byte{
		1, 0, // One player, and you are player zero.  TODO: multiplayer
	}
	bb = append(bb, make([]byte, 2*gamer.MaxPlayers)...) // partials
	bb = append(bb, make([]byte, 2*gamer.MaxPlayers)...) // totals
	bb = append(bb, make([]byte, 2*gamer.MaxPlayers)...) // old_partials
	log.Printf("SendInitializedScores $%04x: len=%d % 3x", gamer.GScore, len(bb), bb)
	// gamer.SendPacket(N_POKE, gamer.GScore, bb)
}

func (gamer *Gamer) Run() {
	Log("================================")
	Try(func() { Discharge(gamer) })
	Enlist(gamer)
	Log("================================")

	inchan := make(chan Packet)
	inpack := &InputPacketizer{
		Conn:  gamer.Conn,
		out:   inchan,
		gamer: gamer,
	}

	defer func() {
		r := recover()
		if r != nil {
			log.Printf("GAMER RUN %v EXITING, CAUGHT %v", gamer, r)
		}
		Try(func() { Discharge(gamer) })
		Try(func() { close(inchan) })
		Try(func() { gamer.Conn.Close() })
	}()

	go inpack.Go()

	gamer.ConsoleSync()
	for {
		gamer.Step(inchan) // doesn't return until Error
	}
}

func NiceHandle(s string) string {
	var buf bytes.Buffer
	s = strings.TrimSpace(s)

	for i := 0; i < 3; i++ {
		if i < len(s) {
			c := s[i]
			if c > 96 {
				c -= 32 // lower to upper
			}
			if 'A' <= c && c <= 'Z' {
				buf.WriteByte(c)
			} else {
				buf.WriteByte('Z')
			}
		} else {
			buf.WriteByte('Z')
		}
	}
	return buf.String()
}

func MCP(conn net.Conn, p uint, pay []byte, hellos map[uint][]byte) {
	Log("MCP remote: %q", conn.RemoteAddr().String())
	Log("MCP p: %d.", p)
	Log("MCP pay: %q = % 3x", pay, pay)
	for k, v := range hellos {
		Log("MCP hello(%04x): %q = % 3x", k, v, v)
	}
	var g *Gamer

	g = &Gamer{
		Conn:   conn,
		Hellos: hellos,

		Handle:  "UNK",
		Name:    "UNKNOWN.NAME",
		Zone:    *GAME_ZONE,
		Airport: "ORD",

		Level:    p,
		Greeting: pay,
		/*
			ConsAddr:   WordFromBytes(pay, 8),
			MaxPlayers: WordFromBytes(pay, 10),
			GScore:     WordFromBytes(pay, 12),
			GWall:      WordFromBytes(pay, 14),
		*/
	}

	if bb, ok := hellos[0]; ok {
		g.Special = string(bb)
	}

	if bb, ok := hellos[HELLO_HANDLE]; ok && len(bb) > 0 {
		g.Handle = NiceHandle(string(bb))
		g.Name = g.Handle
	} else {
		if bb, ok := hellos[0xDF00]; ok && len(bb) >= 256 {
			g.Handle = NiceHandle(string(bb[0xE0:0xE8]))
		}
	}

	if bb, ok := hellos[HELLO_NAME]; ok && len(bb) > 0 {
		g.Name = strings.ToUpper(string(bb))
	} else {
		if bb, ok := hellos[0xDF00]; ok && len(bb) >= 256 {
			g.Name = strings.ToUpper(string(bb[0xE0:0xE8]))
		}
	}

	if bb, ok := hellos[HELLO_AIRPORT]; ok {
		g.Airport = strings.ToUpper(string(bb))
	}

	if bb, ok := hellos[HELLO_ZONE]; ok {
		if len(bb) > 0 {
			// explicit zone provided
			g.Zone = string(bb)
		} else if len(g.Airport) > 0 {
			// use zone of airport
			g.Zone = string(bb)
			if airport, ok := data.AirportMap[g.Airport]; ok {
				g.Zone = airport.Timezone
			} else {
				// Default to Glenside
				g.Zone = "America/Chicago"
			}
		} else {
			// Default to Glenside
			g.Zone = "America/Chicago"
		}
	}

	if location, err := time.LoadLocation(g.Zone); err == nil {
		g.Location = location
	}
	if g.Location == nil {
		g.Location = Value(time.LoadLocation("America/Chicago"))
	}

	g.Trans = transcript.New()
	g.Trans.AppendTime(transcript.PlainString(g.FormatHHMM(time.Now())))

	g.Run()
}

func (g *Gamer) FormatHHMM(t time.Time) string {
	return t.In(g.Location).Format("----- 15:04 -----")
}
