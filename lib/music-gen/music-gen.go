package main

// go run music-gen/music-gen.go -gain 15 -rate 2000 -a 330 -beat 0.3 > music-gen/music-tables.h

import (
    "flag"
    "fmt"
    "math"
)

const (
    c3 = 1*12 + 0
    d3 = 1*12 + 2
    e3 = 1*12 + 4
    f3 = 1*12 + 5
    g3 = 1*12 + 7
    a3 = 1*12 + 9
    b3 = 1*12 + 11

    c4 = 2*12 + 0
    d4 = 2*12 + 2
    e4 = 2*12 + 4
    f4 = 2*12 + 5
    g4 = 2*12 + 7
    a4 = 2*12 + 9
    b4 = 2*12 + 11

    c5 = 3*12 + 0
    d5 = 3*12 + 2
    e5 = 3*12 + 4
    f5 = 3*12 + 5
    g5 = 3*12 + 7
    a5 = 3*12 + 9
    b5 = 3*12 + 11

    WHOLE = 8*1024
    LONG = 4*1024
    SHORT = 2*1024
    REST = 1*1024
    L = 512
)

var BEAT = flag.Float64("beat", 0.5, "length (in seconds) of one beat i.e. a quarter note i.e. a crotchet")
var RATE = flag.Float64("rate", 2500, "samples per second")
var WAVE = flag.Int("wave", 128, "size of wave table")
var GAIN = flag.Float64("gain", 30, "magnitude of sin wave")
var QUIET = flag.Float64("quiet", 0.8, "portion of loudness for quiet")
var A4FREQ = flag.Float64("a", 440.0, "frequence of a4")

var tones0 = []int {
    REST, REST, REST, REST,

    g4, b4, d5, g5, d5, b4, LONG+g4,
    REST, REST, REST, REST,
    g4, a4, b4, c5, d5, e5, 1+f5, g5,
    REST, REST, REST, REST,
    g4, b4, d5, g5, d5, b4, LONG+g4,
    REST, REST, REST, REST,

    // Ode to Joy
    b4, b4, c5, d5, + d5, c5, b4, a4, + g4, g4, a4, b4,
    b4, a4, LONG+a4,

    b4, b4, c5, d5, + d5, c5, b4, a4, + g4, g4, a4, b4,
    a4, g4, LONG+g4,

    a4, a4, b4, g4, 
    a4, SHORT+b4, SHORT+c5, b4, g4,
    a4, SHORT+b4, SHORT+c5, b4, a4,
    g4, a4, d4, L+b4,

    b4, b4, c5, d5, + d5, c5, b4, a4, + g4, g4, a4, b4,
    a4, g4, LONG+g4,

    REST, REST, REST, REST,

    // Ode to Joy
    b4, b4, c5, d5, + d5, c5, b4, a4, + g4, g4, a4, b4,
    b4, a4, LONG+a4,

    b4, b4, c5, d5, + d5, c5, b4, a4, + g4, g4, a4, b4,
    a4, g4, LONG+g4,

    a4, a4, b4, g4, 
    a4, SHORT+b4, SHORT+c5, b4, g4,
    a4, SHORT+b4, SHORT+c5, b4, a4,
    g4, a4, d4, L+b4,

    b4, b4, c5, d5, + d5, c5, b4, a4, + g4, g4, a4, b4,
    a4, g4, LONG+g4,

}

var tones1 = []int {
    REST, REST, REST, REST,

    REST, REST, REST, REST, REST, REST, REST, REST,
    REST, REST, REST, REST,
    REST, REST, REST, REST, REST, REST, REST, REST,
    REST, REST, REST, REST,
    REST, REST, REST, REST, REST, REST, REST, REST,
    REST, REST, REST, REST,

    // Ode to Joy
    REST, REST, REST, REST, REST, REST, REST, REST,
    REST, REST, REST, REST, REST, REST, REST, REST,

    REST, REST, REST, REST, REST, REST, REST, REST,
    REST, REST, REST, REST, REST, REST, REST, REST,

    REST, REST, REST, REST, REST, REST, REST, REST,
    REST, REST, REST, REST, REST, REST, REST, REST,

    REST, REST, REST, REST, REST, REST, REST, REST,
    REST, REST, REST, REST, REST, REST, REST, REST,

    REST, REST, REST, REST,

    // Ode to Joy

    L+WHOLE+g3, L+WHOLE+c3, L+WHOLE+e3, L+WHOLE+d3,
    L+WHOLE+g3, L+WHOLE+c3, L+WHOLE+e3, L+WHOLE+d3,
    L+WHOLE+d4, L+WHOLE+c4, L+WHOLE+b3, L+WHOLE+a3,
    L+WHOLE+g3, L+WHOLE+c3, L+WHOLE+e3, L+WHOLE+d3,
    L+WHOLE+g3,
}

var p = fmt.Printf

func main() {
    flag.Parse()
    EmitWaveSin("Loud", *WAVE, 1.0)
    EmitWaveSin("Quiet", *WAVE, 0.8)
    EmitTonesTable("Tones0", "Loud", tones0)
    EmitTonesTable("Tones1", "Quiet", tones1)
}

func EmitWaveSin(name string, size int, gain float64) {
    p("gword %s[] = {\n", name)
    for i := 0; i < size; i++ {
        phase := float64(i) * (2.0 * math.Pi) / float64(size)
        sine := *GAIN * math.Sin(phase) * gain
        sine += (*GAIN / 4) * math.Sin(2 * phase) * gain
        sine += (*GAIN / 8) * math.Sin(3 * phase)
        sine += (*GAIN / 6) * math.Sin(4 * phase) * gain
        sine += (*GAIN / 10) * math.Sin(5 * phase)
        p("  %d,\n", int(4 * 256 * sine))
    }
    p("};\n")
}

func EmitTonesTable(name string, wav string, tones []int) {
    p("// sizeof %q is %d\n", name, len(tones))
    p("struct tone %s[] = {\n", name)
    for _, t := range tones {
        hold := 0.9
        gap := 0.1
        if t >= WHOLE {
            hold = 3.9
            t -= WHOLE
        }
        if t >= LONG {
            hold = 1.9
            t -= LONG
        }
        if t >= SHORT {
            hold = 0.4
            t -= SHORT
        }
        if t == REST {
            p("  { %d, %d, %s },\n", uint((hold+gap) * *BEAT * *RATE ), 0, wav);
        } else if (t & L) != 0 {
            t -= L
            hold += gap
            gap = 0
            p("  { %d, %d, %s },\n", uint(hold * *BEAT * *RATE ), Stride(t), wav);
        } else {
            if wav == "Loud" {
                p("  { %d, %d, Loud },\n", uint(0.3 * *BEAT * *RATE ), Stride(t));
                p("  { %d, %d, Quiet },\n", uint((hold-0.3) * *BEAT * *RATE ), Stride(t));
            } else {
                p("  { %d, %d, %s },\n", uint(hold * *BEAT * *RATE ), Stride(t), wav);
            }
            p("  { %d, %d, %s },\n", uint(gap * *BEAT * *RATE ), 0, wav);
        }
    }
    p("  { 0 }\n")  // termination record with ttl==0
    p("};\n")
}

func Stride(note int) uint {
    freq := *A4FREQ * math.Pow(2.0, float64(note - a4) / 12.0)
    stride := freq * float64(*WAVE) / *RATE
    return uint(256 * stride)
}
