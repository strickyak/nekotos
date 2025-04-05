package mcp

import (
    "sync"

	. "github.com/strickyak/nekotos/mcp/util"
)

var gamerByHandleMutex sync.Mutex

var gamerByHandle = make(map[string]*Gamer)

func Enlist(gamer *Gamer) {
    //gamerByHandleMutex.Lock()
    //defer gamerByHandleMutex.Unlock()

	handle := Str(gamer)
	_, already := gamerByHandle[handle]
	if already {
		Panic("Handle %q already enlisted", handle)
	}
	gamerByHandle[handle] = gamer

	KernelSendChatf("*** WELCOME %s (%q)", gamer.Handle, gamer.Name)

	Log("@@@@@@@@ Enlisted: %q Now: %s", handle, KeysString(gamerByHandle))

}

func Discharge(gamer *Gamer) {
    //gamerByHandleMutex.Lock()
    //defer gamerByHandleMutex.Unlock()

	handle := Str(gamer)
	delete(gamerByHandle, handle)

	KernelSendChatf("*** BYE %s", gamer.Handle)
	Log("@@@@@@@@ Discharged: %q Now: %s", handle, KeysString(gamerByHandle))
}

func CurrentGamers() map[string]*Gamer {
    //gamerByHandleMutex.Lock()
    //defer gamerByHandleMutex.Unlock()

    var z = make(map[string]*Gamer)
    for k, v := range gamerByHandle {
        z[k] = v
    }
    return z
}

func CommandWho(g *Gamer) {
    for k, v := range CurrentGamers() {
        if v.Room != nil {
            r := v.Room.Number
            KernelSendChatf("* %3s%2d %q", k, r, v.Name)
        } else {
            KernelSendChatf("* %3s   %q", k, v.Name)
        }
    }
}
