package mcp

import (
	. "github.com/strickyak/nekot-coco-microkernel/mcp/util"
)

const MinRoom = 40
const MaxRoom = 99
const NumRooms = MaxRoom - MinRoom + 1

var Rooms [NumRooms]*Room

type Room struct {
	Number  uint
	Title   string
	Members map[string]*Gamer
}

func init() {
	// There are not very many rooms, so no use being lazy.
	// Create them all now.
	for i := uint(MinRoom); i <= MaxRoom; i++ {
		Rooms[i-MinRoom] = &Room{
			Number:  i,
			Title:   Format("Room%d"),
			Members: make(map[string]*Gamer),
		}
	}
}

func FindRoom(num uint) *Room {
	if num < MinRoom || num > MaxRoom {
		Panic("Rooms are numbered %d to %d, so Room%d is invalid",
			MinRoom, MaxRoom, num)
	}
	return Rooms[num-MinRoom]
}

func joinRoom(gamer *Gamer, number uint) *Room {
	room := FindRoom(number)
	room.Members[gamer.Handle] = gamer
	return room
}

func leaveRoom(gamer *Gamer, room *Room) {
	delete(room.Members, gamer.Handle)
}

func CommandJoin(gamer *Gamer, number uint) {
	if gamer.Room != nil {
		leaveRoom(gamer, gamer.Room)
	}
	if number == 0 {
		gamer.Room = nil
	} else {
		gamer.Room = joinRoom(gamer, number)
	}
}
