package transcript

import (
    // "fmt"
)

// To begin with, this is specific to the VDG 32x16 Text mode.
const Stride = 14  // Skip top and bottom lines
const W = 32

type Line [W]byte

type Rec struct {
    lines []Line
}

func (r *Rec) AppendString(s string) {
    for {
        n := len(s)
        if n <= W {
            var tmp Line
            for i:=0; i<n; i++ {
                tmp[i] = s[i]
            }
            for i:=n; i<W; i++ {
                tmp[i] = ' '
            }
            r.lines = append(r.lines, tmp)
        } else {
            r.AppendString(s[:W])
            r.AppendString("+ " + s[W:])
        }
    }
}

func Plain(b byte) byte {
    return 63 & b
}

var BlankLine Line
func init() {
    for i:=0; i<W; i++ {
        BlankLine[i] = ' '
    }
}

func (r *Rec) AppendTime(timestamp string) {
    n := len(timestamp)
    tmp := BlankLine
    tmp[0], tmp[W-1] = Plain('['), Plain(']')
    offset := (W/2) - (n/2)
    for i := 0; i <n; i++ {
        tmp[offset+i] = timestamp[i]
    }

    ll := len(r.lines)
    if ll > 0 && r.lines[ll-1][0] == '[' && r.lines[ll-1][W-1] == ']' {
        r.lines[ll-1] = tmp  // Overwrite trailing timestamp
    } else {
        r.lines = append(r.lines, tmp) // Append new timestamp
    }
}

func (r *Rec) NumPages() int {
    return (len(r.lines) + (Stride-1)) / Stride
}

func (r *Rec) ScreenPage(page int) [Stride]Line {
    var z [Stride]Line
    ll := len(r.lines)

    for j := 0; j < Stride; j++ {
        want := page*Stride+j
        if want < ll {
            z[j] = r.lines[want]
        } else {
            z[j] = BlankLine
        }
    }
    return z
}
func (r *Rec) ScreenTail() [Stride]Line {
    var z [Stride]Line
    ll := len(r.lines)

    for j := 0; j < Stride; j++ {
        if j < ll {
            z[j] = r.lines[j]
        } else {
            z[j] = BlankLine
        }
    }
    return z
}
