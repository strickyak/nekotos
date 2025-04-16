package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"runtime/debug"
	"strings"
	"time"

	"github.com/strickyak/nekotos/mcp"
	. "github.com/strickyak/nekotos/mcp/util"
)

var PORT = flag.Int("port", 2321, "Listen on this TCP port (V41)")

const (
	CMD_HELLO = 1

	CMD_LOG           = 200
	CMD_DATA          = 204
	CMD_ECHO          = 217 // reply with CMD_DATA, with high bits toggled.
	CMD_HELLO_NekotOS = 64
)

func WriteFive(conn net.Conn, cmd byte, n uint, p uint) {
	log.Printf("WriteFive: %d.=%x %x %x", cmd, cmd, n, p)
	_, err := conn.Write([]byte{cmd, Hi(n), Lo(n), Hi(p), Lo(p)}) // == WriteFull
	if err != nil {
		log.Panicf("writeFive: stopping due to error: %v", err)
	}
}
func ReadFive(conn net.Conn) (cmd byte, n uint, p uint) {
	quint := make([]byte, 5)
	log.Printf("===================================")
	_, err := io.ReadFull(conn, quint)
	// err := MyReadFull(conn, quint)
	if err != nil {
		log.Panicf("ReadFive: stopping due to error: %v", err)
	}
	cmd = quint[0]
	n = HiLo(quint[1], quint[2])
	p = HiLo(quint[3], quint[4])
	log.Printf("ReadFive: %d.=%02x %04x %04x", cmd, cmd, n, p)
	return
}

func ReadN(conn net.Conn, n uint) []byte {
	bb := make([]byte, n)
	log.Printf("ReadN...")
	_, err := io.ReadFull(conn, bb)
	// err := MyReadFull(conn, bb)
	if err != nil {
		log.Panicf("ReadN=%d.: stopping due to error: %v", n, err)
	}
	return bb
}

func ReadFiveLoop(conn net.Conn, hellos map[uint][]byte) {
	for {
		cmd, n, p := ReadFive(conn)
		log.Printf("===== ReadFive: cmd=%02x n=%04x p=%04x ........", cmd, n, p)

		switch cmd {
		case CMD_HELLO:
			{
				// Very much like CMD_DATA.
				log.Printf("ReadFive: HELLO $%x @ $%x", n, p)
				if n > 0 {
					data := make([]byte, n)
					_, err := io.ReadFull(conn, data)
					if err != nil {
						log.Panicf("ReadFive: HELLO: stopping due to error: %v", err)
					}
					log.Printf("(HELLO): %q", data)
				}
			}

		case CMD_LOG:
			{
				data := make([]byte, n)
				_, err := io.ReadFull(conn, data)
				if err != nil {
					log.Panicf("ReadFive: DATA: stopping due to error: %v", err)
				}
				log.Printf("ReadFive: LOG %q", data)
			}

		case CMD_DATA: // Sort of a core dump?
			{
				log.Printf("ReadFive: DATA $%x @ $%x", n, p)
				data := make([]byte, n)
				_, err := io.ReadFull(conn, data)
				if err != nil {
					log.Panicf("ReadFive: DATA: stopping due to error: %v", err)
				}
				log.Printf("(DATA): %q", data)
			}

		case CMD_ECHO: // echos back payload with CMD_DATA
			pay := ReadN(conn, n)
			WriteFive(conn, CMD_DATA, n, p)
			conn.Write(pay)

		case CMD_HELLO_NekotOS:
			pay := make([]byte, n)
			_, err := io.ReadFull(conn, pay)
			if err != nil {
				log.Panicf("ReadFive: pay: stopping due to error: %v", err)
			}

			// For CocoIOr to request NekotOS:
			log.Printf("CMD_HELLO_NekotOS %d: %q", p, pay)
			mcp.MCP(conn, p, pay, hellos)

		default:
			log.Panicf("ReadFive: BAD COMMAND $%x=%d.", cmd, cmd)
		} // end switch
	} // end for
} // end ReadFiveLoop

func UpperCleanName(s []byte, maxLen int) string {
	var buf bytes.Buffer
	for _, b := range s {
		if 'A' <= b && b <= 'Z' {
			buf.WriteByte(b)
		} else if '0' <= b && b <= '9' {
			buf.WriteByte(b)
		} else if 'a' <= b && b <= 'z' {
			buf.WriteByte(b - 32)
		} else {
			buf.WriteByte('_')
		}
	}
	z := strings.TrimRight(buf.String(), "_")
	if z == "" {
		z = "EMPTY"
	}
	if !('A' <= z[0] && z[0] <= 'Z') {
		z = "X" + z
	}
	if len(z) > maxLen {
		z = z[:maxLen]
	}
	return z
}

func GetHellos(conn net.Conn) map[uint][]byte {
	dict := make(map[uint][]byte)
	for {
		cmd, n, p := ReadFive(conn)
		log.Printf("GetHellos: $%x $%x $%x", cmd, n, p)
		if cmd != CMD_HELLO {
			log.Panicf("Expected CMD_HELLO, got $%x $%x $%x", cmd, n, p)
		}

		var payload []byte
		if n > 0 {
			payload = ReadN(conn, n)
		}
		dict[p] = payload

		if p == 0 {
			return dict
		}
	}
}

func Serve(conn net.Conn) {
	defer func() {
		r := recover()
		if r != nil {
			log.Printf("Recovered: %q", r)
			debug.PrintStack()
			log.Printf("Closing connection %q: Exception %v", conn.RemoteAddr().String(), r)
		} else {
			log.Printf("Done with connection %q", conn.RemoteAddr().String())
		}
		conn.Close()
	}()

	// TODO, Launch once or twice?
	temp := mcp.Gamer{ // Temporary Gamer, just to Launch Kernel.
		Conn: conn,
	}
	temp.LaunchKernel("bonobo-nekotos") // Becomes GAMES_DIR/"kernel.bonobo-nekotos.decb"

	// You have 10 seconds to say Hello.
	timeoutDuration := 10 * time.Second
	conn.SetReadDeadline(time.Now().Add(timeoutDuration))
	hellos := GetHellos(conn)
	// You pass the test.  No more time limits.
	var noMoreDeadline time.Time // the "zero" value.
	conn.SetReadDeadline(noMoreDeadline)

	greeting, ok := hellos[0]
	if !ok {
		log.Panicf("Missing HELLO for 0")
	}

	if string(greeting) == "bonobo-nekotos" {
		// For Bonobo to request NekotOS:
		log.Printf("GREETING Bonobo %d: %q", 0, greeting)
		mcp.MCP(conn, 0, nil, hellos)
		return
	} else {
		log.Panicf("BAD Greeting %q in bonobo", greeting)
	}
}

func Catch(label string, fn func()) (err string) {
	defer func() {
		r := recover()
		if r != nil {
			err = fmt.Sprintf("%v", r)
			log.Printf("Catch %q: %q", label, err)
		}
	}()
	fn()
	return ""
}

func Listen() {

	l, err := net.Listen("tcp", Format(":%d", *PORT))
	if err != nil {
		log.Panicf("Cannot Listen(): %v", err)
	}
	defer l.Close()
	log.Printf("Waiter listening on TCP port %d", *PORT)

	for {
		conn, err := l.Accept()
		if err != nil {
			log.Panicf("Cannot Accept() connection: %v", err)
		}
		log.Printf("Accepted %q.", conn.RemoteAddr().String())
		go Serve(conn)
	}
}

func main() {
	flag.Parse()
	log.SetFlags(0)
	log.SetPrefix("mcp-bo: ")
	Listen()
}
