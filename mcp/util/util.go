package util

import (
	"bytes"
	"fmt"
	"log"
)

//////////////////////////////////////////////////////
//
//  Utilities

type Ordered interface {
	~byte | ~int | ~uint | ~int64 | ~uint64 | ~rune | ~string
}
type Integer interface {
	~int | ~uint
}

func AssertGE[T Ordered](a, b T) {
	if !(a >= b) {
		log.Panicf("FAILED AssertGE(%v, %v)", a, b)
	}
}

func Hi[T Integer](x T) byte {
	return byte(x >> 8)
}

func Lo[T Integer](x T) byte {
	return byte(x)
}

func HiLo(a, b byte) uint {
	return (uint(a) << 8) | uint(b)
}

func WordFromBytes(bb []byte, offset uint) uint {
	return (uint(bb[offset]) << 8) | uint(bb[offset+1])
}

func Value[T any](value T, err error) T {
	if err != nil {
		log.Panicf("ERROR: %v", err)
	}
	return value
}

func Try(fn func()) (exception string) {
	defer func() {
		r := recover()
		if r != nil {
			exception = Format("%v", r)
			log.Printf("Try ignored: %q", exception)
		}
	}()
	fn()
	return
}

func Str(x any) string {
	return Format("%v", x)
}

func KeysString[K comparable, V any](d map[K]V) string {
	var buf bytes.Buffer
	for k := range d {
		buf.WriteString(Str(k))
	}
	return buf.String()
}

//type Comparable interface {
//int
//uint
//string
//}

var Format = fmt.Sprintf
var Log = log.Printf
var Panic = log.Panicf
