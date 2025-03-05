// The Master Control Program is the Server that Nekot cocos connect to.
// At the moment, this is dispatched from the Lemma Server in frobio,
// upon it receiving a quint with `CMD_HELLO_NEKOT = 64`.
package mcp

import (
	"os"
	"log"
	"net"
	"time"
)

func Value[T any](value T, err error) T {
	if err != nil {
		log.Panicf("ERROR: %v", err)
	}
	return value
}

type Shard struct {
	shard int
}
type Console struct {
}
type LineBuf struct {
}

type Gamer struct {
	conn   net.Conn
	handle string
	name   string

	game  *Game
	shard *Shard
	round uint
	load  *Load

	cons *Console
	line *LineBuf
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

		p := Packet{header[0], MkWord(header[1:3]), MkWord(header[3:5]), nil}
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
		o.out <- p
	}
	o.out <- Packet{0, 0, 0, nil} // End Sentinel
	close(o.out)
}

type Keystroke struct {
	matrix [8]byte
	ascii  byte
}

type KeystrokeMunger struct {
	in    chan Packet
	out   chan byte // Ascii Char
	gamer *Gamer
}

func (o *KeystrokeMunger) Go() {
	/*
	   	for {
	   	    p, err := <-o.in
	   	    if err != nil {
	   	        log.Panicf("KeystrokeMunger.Go(): chan read got error: %v", err)
	   	    }
	   	    if p.c != CMD_TODO................ TODO XXX ............

	   }
	*/
}

type Quint struct {
	c byte
	n uint
	p uint
}

func (q Quint) Write(conn net.Conn) {
	var buf [5]byte
	buf[0] = q.c
	buf[1] = byte(q.n >> 8)
	buf[2] = byte(q.n)
	buf[3] = byte(q.p >> 8)
	buf[4] = byte(q.p)

	_ = Value(conn.Write(buf[:]))
}

func (g *Gamer) WritePacket(c byte, p uint, bb []byte) {
	q := Quint{c, uint(len(bb)), p}
	q.Write(g.conn)

	_ = Value(g.conn.Write(bb))
}

func (g *Gamer) Step() {
	if false {
		for c := 'A'; c <= 'A'; c++ {
			for addr := 0x220; addr < 0x280; /*0x300*/ addr += 32 {
				var buf [37]byte
				buf[0] = 66                        // Poke
				buf[1], buf[2] = 0, 32             // Length
				buf[3], buf[4] = 2, byte(addr&255) // Length

				for i := 0; i < 32; i++ {
					buf[i+5] = byte(int(c) + i)
				}

				_ = Value(g.conn.Write(buf[:]))
				time.Sleep(time.Second)
			}
		}
	}

    // blue
	blue := Value(os.ReadFile("/tmp/_blue.decb"))
	g.SendGameAndLaunch(blue)
	time.Sleep(1000000 * time.Second)
	log.Printf("WWWWWWWWW TTTTTTTTTTTTTT FFFFFFFFFFFFFF");

	time.Sleep(3 * time.Second)
	g.WritePacket(68, 0, nil)
	time.Sleep(1 * time.Second)
	g.WritePacket(68, 0, nil)
	time.Sleep(1000000 * time.Second)

    // green
	green := Value(os.ReadFile("/tmp/_green.decb"))
	g.SendGameAndLaunch(green)

	time.Sleep(3 * time.Second)
	g.WritePacket(68, 0, nil)
	time.Sleep(1 * time.Second)
	g.WritePacket(68, 0, nil)
	time.Sleep(1 * time.Second)
}

func (g *Gamer) SendGameAndLaunch(bb []byte) {
	for len(bb) >= 5 {
		c := bb[0]
		n := (uint(bb[1]) << 8) | uint(bb[2])
		p := (uint(bb[3]) << 8) | uint(bb[4])
		log.Printf("SendGameAndLaunch: %x %x %x (len=%d)", c, n, p, len(bb))
		bb = bb[5:]

		switch c {
		case 0:
			g.WritePacket(66, p, bb)
			bb = bb[n:]

		case 255:
			g.WritePacket(68, p, nil)

		default:
			panic(bb[0])
		}
	}
}

func (gamer *Gamer) Run() {
	for {
		gamer.Step()
	}
}

func MCP(conn net.Conn) {
	g := &Gamer{
		conn:   conn,
		handle: "YAK",
		name:   "YAK",
	}
	g.Run()
}
