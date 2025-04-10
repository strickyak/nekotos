package mcp

import (
	"log"

	. "github.com/strickyak/nekotos/mcp/util"
)

const MaxPlayers = 8
const MinRoom = 40
const MaxRoom = 99
const NumRooms = MaxRoom - MinRoom + 1

var Rooms [NumRooms]*Room

type Room struct {
	Number   uint
	Title    string
	members  [MaxPlayers]*Gamer
	Partials [MaxPlayers][MaxPlayers]int16
}

func (r *Room) PlayerNumber(g *Gamer) byte {
	for i, m := range r.members {
		if m == g {
			return byte(i)
		}
	}
	return 255 // not found
}

// TODO: if some left, these may be in the wrong order.
func (r *Room) Members() (z []*Gamer) {
	for _, m := range r.members {
		if m != nil {
			z = append(z, m)
		}
	}
	return
}

func (r *Room) AddMember(g *Gamer) {
	for i, m := range r.members {
		if m == nil {
			r.members[i] = g
			return
		}
	}
	log.Panicf("Too many members: room %d.", r.Number)
}

// TODO: destory room if someone leaves?
// This leaves player numbers and slots inconsistent.
func (r *Room) DeleteMember(g *Gamer) {
	for i, m := range r.members {
		if m == g {
			r.members[i] = nil
			return
		}
	}
}

func init() {
	// There are not very many rooms, so no use being lazy.
	// Create them all now.
	for i := uint(MinRoom); i <= MaxRoom; i++ {
		Rooms[i-MinRoom] = &Room{
			Number: i,
			Title:  Format("Room%d"),
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
	room.AddMember(gamer)
	return room
}

func leaveRoom(gamer *Gamer, room *Room) {
	room.DeleteMember(gamer)
}

func CommandJoin(gamer *Gamer, number uint) {
	if gamer.Room != nil {
		leaveRoom(gamer, gamer.Room)
	}
	if number == 0 {
		gamer.Room = nil
	} else {
		if len(FindRoom(number).Members()) >= MaxPlayers {
			Panic("Room %d is full", number)
		}
		gamer.Room = joinRoom(gamer, number)
	}
}

const SCORES_LEN = 2 + 1 + 2*MaxPlayers + 1 + 2*MaxPlayers

func CommandRun(gamer *Gamer, ww []string) {
	if len(ww) != 1 {
		Panic("Usage: /RUN gamename")
	}
	gamename := ww[0]
	r := gamer.Room
	if r == nil {
		/*
		   scores := make([]byte, SCORES_LEN)
		   scores[0] = 1  // number_of_players
		   if gamer.GScore == 0 {
		       Panic("Member %q has no Scores address", gamer)
		   }
		   gamer.SendPokeMemory(gamer.GScore, scores)
		*/
		gamer.LaunchGame(gamename)
	} else {
		mems := r.Members()
		pnum := r.PlayerNumber(gamer)

		Log("Gamer %q in room %d is pnum %d, mems %v", gamer, r.Number, pnum, mems)

		if pnum != 0 {
			Panic("Only %q can /run games in room %d", mems[0], r.Number)
		}

		// Launch for everybody.
		for _, m := range mems {
			/*
			   scores := make([]byte, SCORES_LEN)
			   scores[0] = byte(len(mems))  // number_of_players
			   scores[1] = r.PlayerNumber(m)
			   if m.GScore == 0 {
			       Panic("Member %q has no Scores address", m)
			   }
			   Log("Sending Scores Ram to player %d%q: % 3x", i, m, scores)
			   m.SendPokeMemory(m.GScore, scores)
			*/
			m.LaunchGame(gamename)
		}
	}
}
