package transcript

import (
// "time"

// . "github.com/strickyak/nekotos/mcp/util"
)

// To begin with, this is specific to the VDG 32x16 Text mode.
const Stride = 10 // Was 14 // Skip top and bottom lines
const W = 32

type Line [W]byte

type Rec struct {
	Lines []Line
}

func PlainChar(b byte) byte {
	return 63 & b
}

func PlainString(s string) string {
	var z []byte
	for i := 0; i < len(s); i++ {
		x := s[i]
		if x < 32 {
			x = '#' // control char
		} else if x < 64 {
			x = x // numbers and symbols
		} else if x < 96 {
			x = x - 64 // lowercase
		} else if x < 128 {
			x = x - 96 // uppercase
		} else {
			x = x // semigraphics
		}
		z = append(z, x)
	}
	return string(z)
}

var BlankLine Line

func init() {
	for i := 0; i < W; i++ {
		BlankLine[i] = ' '
	}
}

func New() *Rec {
	rec := &Rec{}
	return rec
}

func (r *Rec) AppendString(s string) {
	for {
		n := len(s)
		if n <= W {
			var tmp Line
			for i := 0; i < n; i++ {
				tmp[i] = s[i]
			}
			for i := n; i < W; i++ {
				tmp[i] = ' '
			}
			r.Lines = append(r.Lines, tmp)
			break
		} else {
			r.AppendString(s[:W])
			r.AppendString("+ " + s[W:])
		}
	}
}

func (r *Rec) AppendTime(timestamp string) {
	n := len(timestamp)
	tmp := BlankLine
	tmp[0], tmp[W-1] = PlainChar('['), PlainChar(']')
	offset := (W / 2) - (n / 2)
	for i := 0; i < n; i++ {
		tmp[offset+i] = timestamp[i]
	}

	ll := len(r.Lines)
	if ll > 0 && r.Lines[ll-1][0] == '[' && r.Lines[ll-1][W-1] == ']' {
		r.Lines[ll-1] = tmp // Overwrite trailing timestamp
	} else {
		r.Lines = append(r.Lines, tmp) // Append new timestamp
	}
}

func (r *Rec) NumPages() int {
	return (len(r.Lines) + (Stride - 1)) / Stride
}

// pages are 1 to NumPages. 0 means ScreenTail.
func (r *Rec) ScreenPage(page int) [Stride]Line {
	if page == 0 {
		return r.ScreenTail()
	}
	var z [Stride]Line
	ll := len(r.Lines)

	for j := 0; j < Stride; j++ {
		want := (page-1)*Stride + j
		if want < ll {
			z[j] = r.Lines[want]
		} else {
			z[j] = BlankLine
		}
	}
	return z
}
func (r *Rec) ScreenTail() [Stride]Line {
	var z [Stride]Line
	ll := len(r.Lines)

	i := 0
	for j := ll - Stride; j < ll; j++ {
		if j < 0 {
			z[i] = BlankLine
		} else {
			z[i] = r.Lines[j]
		}
		i++
	}
	return z
}
