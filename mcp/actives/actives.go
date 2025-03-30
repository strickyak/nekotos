package actives

import (
	//"bufio"
	//"fmt"
	//"log"
	//"path/filepath"
	//"sort"
	//"strings"
	//"time"


	// for Assert, Value
	. "github.com/strickyak/nekot-coco-microkernel/mcp/util"
)

type A struct {
	GamerByHandle map[string]any
	RoomByNumber  map[uint]*Room
}

var Actives = newActives()

func newActives() *A {
	return &A{
		GamerByHandle: make(map[string]any),
	}
}

func Enlist(gamer any) {
	handle := Str(gamer)
	_, already := Actives.GamerByHandle[handle]
	if already {
		Panic("Handle %q already enlisted", handle)
	}
	Actives.GamerByHandle[handle] = gamer

    Log("@@@@@@@@ Enlisted: %q Now: %s", handle, KeysString(Actives.GamerByHandle))

}

func Discharge(gamer any) {
	handle := Str(gamer)
	delete(Actives.GamerByHandle, handle)

    Log("@@@@@@@@ Discharged: %q Now: %s", handle, KeysString(Actives.GamerByHandle))
}

type Room struct {
	Number  uint
	Title   string
	Members map[string]any
}

func GamerJoinRoom(gamer any, number uint) {
}

func GamerLeaveRoom(gamer any, number uint) {
	// number should't be required,
	// but it can be checked.
}
