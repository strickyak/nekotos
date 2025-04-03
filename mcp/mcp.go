// The Master Control Program is the Server that Nekot cocos connect to.
// At the moment, this is dispatched from the Lemma Server in frobio,
// upon it receiving a quint with `CMD_HELLO_NEKOT = 64`.
package mcp

import (
	"flag"
	"log"
	"net"
	"os"
	"strings"
	"sync"
	"time"

	"github.com/strickyak/nekot-coco-microkernel/mcp/transcript"
	. "github.com/strickyak/nekot-coco-microkernel/mcp/util"
)

var GAMES_DIR = flag.String("games_dir", "/tmp", "where .games files are located")
var GAME_ZONE = flag.String("zone", "America/New_York", "linux time zone location")

const (
	N_CLOSED  = 63
	N_HELLO   = 64
	N_MEMCPY  = 65
	N_POKE    = 66
	N_START   = 68
	N_KEYSCAN = 69
	N_CONTROL = 70
	CMD_LOG   = 200
)

type Gamer struct {
	Conn      net.Conn
	SendMutex sync.Mutex
	Hellos    map[uint][]byte

	Handle string
	Name   string
	Room   *Room

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

	Level      uint   // Original HELLO `p` parameter
	Hello      []byte // Original HELLO payload
	NekotHash  []byte
	ConsAddr   uint
	MaxPlayers uint
	GWall      uint
	GScore     uint
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
			case N_HELLO:
				Log("%q HELLO(%d) [%d] % 3x", g, p.p, len(p.pay), p.pay)
				if p.p == 1 && len(p.pay) == 16 && string(p.pay[:6]) == "nekot1" {
					g.ConsAddr = WordFromBytes(p.pay, 8)
					g.MaxPlayers = WordFromBytes(p.pay, 10)
					g.GScore = WordFromBytes(p.pay, 12)
					g.GWall = WordFromBytes(p.pay, 14)
					now := g.SendWallTime()
					log.Printf("HELLO(1) sent Wall Time: %v", now)
				} else if p.p == 2 && len(p.pay) == 8 {
					g.NekotHash = p.pay
					log.Printf("HELLO(2) from NekotHash % 3x", p.pay)
					g.ConsoleSync()
				} else {
					log.Panicf("unknown HELLO(%d): % 3x", p.p, p.pay)
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

		decb := Value(os.ReadFile(filename))
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
func (g *Gamer) ExecuteSlashCommand(s string) {
	/*
	   ww := strings.Split(s, " ")
	   n := len(ww)
	   cmd := ww[0]

	   	if strings.HasPrefix(cmd, "J") {
	   	    if n==2 && ww[1]=="0" {
	   	        g.LeaveRoom();
	   	    } else if n==1 {
	   	        g.LeaveRoom();
	   	    } else if n==2 {
	   	        x := strconv.ParseUint(ww[1], 64, 10)
	   	        g.JoinRoom(int(x))
	   	    }
	   	}
	*/
}

func (g *Gamer) ReadVersionedGameFile(basename string) []byte {
	filename := Format("%s/%s.%02x.game", *GAMES_DIR, basename, g.NekotHash)
	log.Printf("%v ReadVersionedGameFile: %q", g, filename)
	return Value(os.ReadFile(filename))
}

func (g *Gamer) EnterLine() {
	s := string(g.Line)
	log.Printf("GAMER %q LINE %q\n", g.Handle, s)
	g.PrintLine(transcript.PlainString(s))

	if strings.HasPrefix(s, "/") {
		g.ExecuteSlashCommand(s[1:])
	} else if s == "0" {
		log.Printf("-> STOP <-")
		g.SendPacket(N_START, 0, nil)
	} else if s == "C" {
		decb := g.ReadVersionedGameFile("clock")
		g.SendGameAndLaunch(decb)
	} else if s == "R" {
		decb := g.ReadVersionedGameFile("red")
		g.SendGameAndLaunch(decb)
	} else if s == "G" {
		decb := g.ReadVersionedGameFile("green")
		g.SendGameAndLaunch(decb)
	} else if s == "B" {
		decb := g.ReadVersionedGameFile("blue")
		g.SendGameAndLaunch(decb)
	} else if s == "L" {
		decb := g.ReadVersionedGameFile("life")
		g.SendGameAndLaunch(decb)
	} else if s == "8" {
		decb := g.ReadVersionedGameFile("lib8")
		g.SendGameAndLaunch(decb)
	} else if s == "4" {
		decb := g.ReadVersionedGameFile("forth")
		g.SendGameAndLaunch(decb)
	} else if s == "S" {
		decb := g.ReadVersionedGameFile("spacewar")
		g.SendGameAndLaunch(decb)
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
	location, err := time.LoadLocation(*GAME_ZONE)
	if err != nil {
		location = time.UTC
		log.Printf("BAD GAME TIME ZONE %q -- USING UTC INSTEAD", *GAME_ZONE)
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

func MCP(conn net.Conn, p uint, pay []byte, hellos map[uint][]byte) {
	var g *Gamer

	if len(pay) > 0 {
		g = &Gamer{
			Conn:       conn,
			Handle:     "YAK",
			Hellos:     hellos,
			Name:       "YAK",
			Level:      p,
			Hello:      pay,
			Trans:      transcript.New(),
			ConsAddr:   WordFromBytes(pay, 8),
			MaxPlayers: WordFromBytes(pay, 10),
			GScore:     WordFromBytes(pay, 12),
			GWall:      WordFromBytes(pay, 14),
		}
	} else {
		g = &Gamer{
			Conn:   conn,
			Handle: "YAK",
			Hellos: hellos,
			Name:   "YAK",
			Level:  p,
			Hello:  pay,
			Trans:  transcript.New(),
			// ConsAddr:   WordFromBytes(pay, 8),
			// MaxPlayers: WordFromBytes(pay, 10),
			// GWall:      WordFromBytes(pay, 12),
			// GScore:     WordFromBytes(pay, 14),
		}
	}

	Log("MCP: %#v", *g)

	g.Run()
}
