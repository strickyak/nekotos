package mcp

import (
	"github.com/strickyak/nekotos/mcp/transcript"
	. "github.com/strickyak/nekotos/mcp/util"
)

func KernelSendChat(s string) {
	msg := transcript.PlainString(s)
	for _, g := range GamerByHandle {
		g.PrintLine(msg)
	}
}

func GamerSendChat(g *Gamer, s string) {
	msg := transcript.PlainString(
		Format("%3s> %s", g.Handle, s))

	for _, g := range GamerByHandle {
		g.PrintLine(msg)
	}
}
