// The Master Control Program is the Server that Nekot cocos connect to.
// At the moment, this is dispatched from the Lemma Server in frobio,
// upon it receiving a quint with `CMD_HELLO_NEKOT = 64`.
package mcp

import (
	"fmt"
	"log"
	"net"
	"os"
	"sync"
	"time"
)

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
	conn      net.Conn
	sendMutex sync.Mutex
	hellos    map[uint][]byte

	handle string
	name   string

	// game  *Game
	// shard *Shard
	// round uint
	// load  *Load

	// cons *Console
	// line *LineBuf

	keys [8]byte
	line []byte

	console [14][32]byte

	level      uint   // Original HELLO `p` parameter
	hello      []byte // Original HELLO payload
	consAddr   uint
	maxPlayers uint
	gWall      uint
	gScore     uint
}

func (o Gamer) String() string {
	return o.handle
}

type Game struct {
	name   string
	source string
}

type Load struct {
	binary []byte
	addr   uint
	entry  uint

	done  uint
	total uint
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
	conn  net.Conn
	out   chan<- Packet
	gamer *Gamer
}

func (o *InputPacketizer) Go() {
	defer func() {
		r := recover()
		if r != nil {
			log.Printf("PANIC: user %q error %v", o.gamer.handle, r)
			close(o.out)
			o.conn.Close()
		}
	}()

	log.Printf("InputPacketizer GO...")
	for {
		// Get Header
		var header [5]byte

		n, err := o.conn.Read(header[:])
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

		n, err = o.conn.Read(p.pay)
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
	g.sendMutex.Lock()
	log.Printf("mcp Gamer.SendPacket c=$%x=%d. p=$%x=%d. len=$%x=%d. ( % 3x )", c, c, p, p, n, n, bb)

	buf := []byte{c, byte(n >> 8), byte(n), byte(p >> 8), byte(p)}
	buf = append(buf, bb...)
	_ = Value(g.conn.Write(buf))
	g.sendMutex.Unlock()
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
				log.Printf("N1LOG: %q %q", g.handle, p.pay)
			case N_KEYSCAN:
				g.KeyScanHandler(p.pay)
			case N_CONTROL:
				g.ControlRequestHandler(p.p, p.pay)
			default:
				log.Printf("WUT? default PPI: %v", p)
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

func (g *Gamer) ControlRequestHandler(p uint, pay []byte) {
	switch p {
	case 'a': // Game Abort
		why := ExtractCString(pay)
		log.Printf("%q Game Aborted: %q\n", g, why)
		g.PrintLine(Format("*** GAME ABORTED: %s", why))
		g.SendPacket(N_START, 0, nil)

	case 'c': // Game Chain
		filename := ExtractCString(pay)
		log.Printf("%q CHAIN => %q <=\n", g, filename)
		g.PrintLine(Format("*** GAME CHAIN TO %s", filename))
		g.SendPacket(N_START, 0, nil)

		decb := Value(os.ReadFile(filename))
		g.SendGameAndLaunch(decb)

	case 'o': // Game Over
		why := ExtractCString(pay)
		log.Printf("%q Game Over: %q\n", g, why)
		g.PrintLine(Format("*** GAME OVER: %s", why))
		g.SendPacket(N_START, 0, nil)

	case 'L': // Logging
		log.Printf("N1Log: %q logs %q", g.handle, pay)

	case 'S': // Partial Scoring
		// Handle Partial Scores
		log.Printf("N1: %q scores % 3x", g.handle, pay)

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
		prev := g.keys[i]
		if prev != x {
			// This byte changed.
			y := (x ^ prev) & x // bits added.
			ch = WhatChar(i, y)
			if ch != 0 {
				log.Printf("KeyScanHandler: ch=%d(%c) y=%x prev=%x i=%x bb=(%02x) keys=(%02x)", ch, ch, y, prev, i, bb, g.keys[:])
				break
			}
		}
	}

	copy(g.keys[:], bb) // Save keyscan.
	if ch != 0 {
		g.InkeyHandler(ch)
	}
}

func (g *Gamer) InkeyHandler(ch byte) {
	if ' ' <= ch && ch <= '~' {
		g.line = append(g.line, ch)
	} else if ch == 8 { // backspace
		if len(g.line) > 0 {
			g.line = g.line[:len(g.line)-1] // remove last char
		}
	} else if ch == 10 { // enter
		g.ExecuteLine()
		g.line = nil
	}
	g.DrawLineOnStatusLine()
}
func (g *Gamer) ExecuteLine() {
	s := string(g.line)
	log.Printf("GAMER %q EXEC(%q)\n", g.handle, s)
	g.PrintLine(s)

	if s == "0" {
		log.Printf("-> STOP <-")
		g.SendPacket(N_START, 0, nil)
	} else if s == "R" {
		decb := Value(os.ReadFile("/tmp/red.game"))
		log.Printf("=> BLUE <=\n")
		g.SendGameAndLaunch(decb)
	} else if s == "G" {
		decb := Value(os.ReadFile("/tmp/green.game"))
		log.Printf("=> BLUE <=\n")
		g.SendGameAndLaunch(decb)
	} else if s == "B" {
		decb := Value(os.ReadFile("/tmp/blue.game"))
		log.Printf("=> BLUE <=\n")
		g.SendGameAndLaunch(decb)
	} else if s == "L" {
		decb := Value(os.ReadFile("/tmp/life.game"))
		log.Printf("=> LIFE <=\n")
		g.SendGameAndLaunch(decb)
	} else if s == "8" {
		decb := Value(os.ReadFile("/tmp/lib8.game"))
		log.Printf("=> LIB8 <=\n")
		g.SendGameAndLaunch(decb)
	} else if s == "4" {
		decb := Value(os.ReadFile("/tmp/forth.game"))
		log.Printf("=> FORTH <=\n")
		g.SendGameAndLaunch(decb)
	} else if s == "S" {
		decb := Value(os.ReadFile("/tmp/spacewar.game"))
		log.Printf("=> SPACEWAR <=\n")
		g.SendGameAndLaunch(decb)
	}
}

func (g *Gamer) DrawLineOnStatusLine() {
	const BlueBottom = 0xA3
	const RedBox = 0xFF
	var bar [32]byte
	n := len(g.line)
	for i := 0; i < 32; i++ {
		if 1 <= i && i <= n {
			bar[i] = 63 & g.line[i-1]
		} else if i == n+1 {
			bar[i] = RedBox
		} else {
			bar[i] = BlueBottom
		}
	}
	g.SendPokeMemory(0x03E0, bar[:])
}

func (g *Gamer) PrintLine(s string) {
	for i := 0; i < 13; i++ {
		g.console[i] = g.console[i+1]
	}
	g.SendMemCopy(0x0220, 0x0240, 13*32)
	for j := 0; j < 32; j++ {
		if j < len(s) {
			ch := s[j]
			if 64 <= ch && ch <= 95 {
				g.console[13][j] = ch - 64
			} else if 96 <= ch && ch <= 127 {
				g.console[13][j] = ch - 96
			} else {
				g.console[13][j] = ch
			}
		} else {
			g.console[13][j] = 32
		}
	}

	g.SendPokeMemory(0x0220+13*32, g.console[13][:])
	// for i := uint(0); i < 14; i++ {
	// g.SendPokeMemory(0x0220+i*32, g.console[i][:])
	// }
}

func (gamer *Gamer) Step(inchan chan Packet) {
	gamer.HandlePackets(inchan)  // doesn't return until Error
}

// SendGameAndLaunch takes the contents of a DECB binary,
// and pokes it into the Coco.
func (gamer *Gamer) SendGameAndLaunch(bb []byte) {
	// Flip back to Shell mode, so you're not executing the old game
	// while loading the new game.
	gamer.SendPacket(N_START, 0, nil)
	gamer.SendInitializedScores()

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

func (gamer *Gamer) WallTimeBytes(addMe time.Duration) []byte {
	location, _ := time.LoadLocation("America/New_York")
	now := time.Now().Add(addMe).In(location)
	y, m, d := now.Date()
	hr, minute, sec := now.Hour(), now.Minute(), now.Second()
	weekday := Weekdays[now.Weekday()]
	month := Months[m]
	wall := []byte{
		byte(sec), byte(minute), byte(hr),
		byte(d), byte(m), byte(y - 2000),
		weekday[0], weekday[1], weekday[2], 0,
		month[0], month[1], month[2], 0,
	}
	return wall
}
func (gamer *Gamer) SendWallTime() {
	bb := gamer.WallTimeBytes(ZeroHours)
	bb = append(bb, gamer.WallTimeBytes(TwentyFourHours)[3:]...) // omit tomorrow's h, m, s
	log.Printf("NOT SendWallTime $%04x: len=%d. % 3x", gamer.gWall, len(bb), bb)
	// gamer.SendPacket(N_POKE, gamer.gWall, bb)
}

func (gamer *Gamer) SendInitializedScores() {
	bb := []byte{
		1, 0, // One player, and you are player zero.  TODO: multiplayer
	}
	bb = append(bb, make([]byte, gamer.maxPlayers)...) // partials
	bb = append(bb, make([]byte, gamer.maxPlayers)...) // totals
	bb = append(bb, make([]byte, gamer.maxPlayers)...) // old_partials
	log.Printf("NOT SendInitializedScores $%04x: len=%d % 3x", gamer.gScore, len(bb), bb)
	// gamer.SendPacket(N_POKE, gamer.gScore, bb)
}

func (gamer *Gamer) Run() {
	inchan := make(chan Packet)
	inpack := &InputPacketizer{
		conn:  gamer.conn,
		out:   inchan,
		gamer: gamer,
	}
	go inpack.Go()

	gamer.SendWallTime()
	for {
		gamer.Step(inchan)  // doesn't return until Error
	}
}

func wordFromBytes(bb []byte, offset uint) uint {
	return (uint(bb[offset]) << 8) | uint(bb[offset+1])
}

func MCP(conn net.Conn, p uint, pay []byte, hellos map[uint][]byte) {
	var g *Gamer

	if len(pay) > 0 {
		g = &Gamer{
			conn:       conn,
			handle:     "YAK",
			hellos:     hellos,
			name:       "YAK",
			level:      p,
			hello:      pay,
			consAddr:   wordFromBytes(pay, 8),
			maxPlayers: wordFromBytes(pay, 10),
			gWall:      wordFromBytes(pay, 12),
			gScore:     wordFromBytes(pay, 14),
		}
	} else {
		g = &Gamer{
			conn:   conn,
			handle: "YAK",
			hellos: hellos,
			name:   "YAK",
			level:  p,
			hello:  pay,
			// consAddr:   wordFromBytes(pay, 8),
			// maxPlayers: wordFromBytes(pay, 10),
			// gWall:      wordFromBytes(pay, 12),
			// gScore:     wordFromBytes(pay, 14),
		}
	}

	for i := 0; i < 14; i++ {
		for j := 0; j < 32; j++ {
			g.console[i][j] = 32
		}
	}

	g.Run()
}

func Value[T any](value T, err error) T {
	if err != nil {
		log.Panicf("ERROR: %v", err)
	}
	return value
}
func Format(f string, args ...any) string {
	return fmt.Sprintf(f, args...)
}
