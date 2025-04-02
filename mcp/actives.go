package mcp

import (
	. "github.com/strickyak/nekot-coco-microkernel/mcp/util"
)

var GamerByHandle = make(map[string]*Gamer)

func Enlist(gamer *Gamer) {
	handle := Str(gamer)
	_, already := GamerByHandle[handle]
	if already {
		Panic("Handle %q already enlisted", handle)
	}
	GamerByHandle[handle] = gamer

	Log("@@@@@@@@ Enlisted: %q Now: %s", handle, KeysString(GamerByHandle))

}

func Discharge(gamer *Gamer) {
	handle := Str(gamer)
	delete(GamerByHandle, handle)

	Log("@@@@@@@@ Discharged: %q Now: %s", handle, KeysString(GamerByHandle))
}
