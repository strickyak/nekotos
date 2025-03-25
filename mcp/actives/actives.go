package actives

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"path"
	"path/filepath"
	"sort"
	"strings"
	"time"

	"github.com/strickyak/nekot-coco-microkernel/mcp"

	// for Assert, Value
	. "github.com/strickyak/frobio/frob3/lemma/util"
)

type A struct {
    GamerByHandle map[string]*mcp.Gamer
    RoomByNumber map[uint]*Room
}

var Actives = newActives()

func newActives() *A {
    return &A{
        handles: make(map[string]*mcp.Gamer)
    }
}

func Enlist(gamer *mcp.Gamer) {
    handle := gamer.Handle
    existing, already := Actives.GamerByHandle[handle]
    if already {
        Discharge(existing)
    }
    Actives.GamerByHandle[handle] = gamer

}

func Discharge(gamer *mcp.Gamer) {
    Try(mcp.Kick(gamer))
    Try(gamer.Conn.Close())
    gamer.Conn = nil
}

func Try(func fn()) {
    defer func() {
        r := recover()
        if r != nil {
            log.Printf("Try ignored: %q", r)
        }
    }()
    fn()
}

type Room struct {
    Number  uint
    Title   string
    Members map[string]*mcp.Gamer
}

func GamerJoinRoom(gamer *mcp.Gamer, number uint) {
}

func GamerLeaveRoom(gamer *mcp.Gamer, number uint) {
    // number should't be required,
    // but it can be checked.
}
