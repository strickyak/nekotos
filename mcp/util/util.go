package util

import (
	"fmt"
	"log"
)

//////////////////////////////////////////////////////
//
//  Utilities

func Hi(x uint) byte {
	return 255 & byte(x>>8)
}

func Lo(x uint) byte {
	return 255 & byte(x)
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

var Format = fmt.Sprintf
var Log = log.Printf
