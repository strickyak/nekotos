package mcp

import (
	"log"

	"github.com/strickyak/nekotos/mcp/transcript"
	. "github.com/strickyak/nekotos/mcp/util"
)

func KernelSendChatf(format string, args ...any) {
	KernelSendChat(Format(format, args...))
}

func KernelSendChat(s string) {
	log.Printf("KernelSendChat: %q", s)

	msg := transcript.PlainString(s)
	for _, g := range CurrentGamers() {
		Try(func() {
			g.PrintLine(msg)
		})
	}
}

func GamerSendChat(g *Gamer, s string) {
	log.Printf("GamerSendChat %q: %q", g.Handle, s)

	msg := transcript.PlainString(
		Format("%3s> %s", g.Handle, s))

	for _, g := range CurrentGamers() {
		Try(func() {
			g.PrintLine(msg)
		})
	}
}
