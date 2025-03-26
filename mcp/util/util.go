package util

import (
	"fmt"
	"log"
)

//////////////////////////////////////////////////////
//
//  Utilities

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
