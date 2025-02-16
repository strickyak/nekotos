package mcp

import (
    "net"
    "log"
)

type Shard struct {
    shard   int
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
    HEAD = 1
    FULL = 2
)

// MkWord turns a 2-byte slice into BigEndian 2-byte word for Coco.
func MkWord(b []byte) uint {
    hi := uint(b[0]);
    lo := uint(b[1]);
    return (hi<<8) | lo
}
// Packet has a 5 byte header (1B c, 2B n, 2B p) and arbitrary payload.
type Packet struct {
    c byte
    n uint
    p uint
    pay     []byte
}
type InputPacketizer struct {
	conn   net.Conn
    out     chan <-Packet
    gamer  *Gamer
}
func (o InputPacketizer) Go() {
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
    o.out <- Packet{0, 0, 0, nil}  // End Sentinel
    close(o.out)
}
type KeystrokeMunger struct {
}

func (g *Gamer) Step() {
    for value := 'A'; value <= 'Z'; value++ {
        for addr := 0x220; addr < 0x300; addr += 32 {
            var buf [37]byte
            buf[0] = 66 // Poke
            buf[1], buf[2] = 0, 32 // Length
            buf[3], buf[4] = 2, byte(addr&255) // Length
            for i := 0; i<32; i++ {
                buf[i+5] = byte(value)
            }
            _, err := g.conn.Write(buf[:])
            if err != nil {
                panic(err)
            }
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
        conn: conn,
        handle: "YAK",
        name: "YAK",
    }
    g.Run()
}
