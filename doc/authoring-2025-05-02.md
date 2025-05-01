# Authoring Realtime Multi-Coco Games for NekotOS

## NekotOS Overview

Networked Realtime Multi-Coco Gaming OS.

### Networked:

* Relatively-Fast Network Hardware
* fast for a 1Mhz 8-bit Machine
* maybe a dozen 64-byte packets/sec

### Realtime:

* 60 Hz VSYNC interrupt
* Drives network, background tasaks
* Provides timing for Game Dyanamics

### Multi-Coco:

* requires a Single client TCP connection to a common server (MCP)
* chat with all others connected
* team up with other players, for a game shard
* get help
* collaborative debugging

### Gaming:

* "Game" is what we call Apps, Programs, or Processes
* Games span across multiple Cocos ("players" in a "shard")
* Games could also be clocks, galleries, movie players, info kiosks

### OS:

* Minimalist, microkernel-ish, under 4KB
* Network is its heaviest component
* Much OS functionality pushed to the MCP server
* Kernel runs during 60Hz interrupt service
* Missing: stdin, stdout, disk

### Retro Purity:

* Game play/dynamics runs fully on the Cocos.
* Server copies small network packets from/to cocos
* Server provides peripheral functionality:
* -- Chat, Commands, Rendez-Vous, Game Software, Downloadable Content,
* -- Top Scores, Wall Clock, (future) Random Seeds

### Initial Hardware:

* Coco 1, 2, or 3
* At least 16KB RAM (Future: 32K, 64K, 128K RAM)
* 6809/6309, VDG graphics, SAM, Coco Keyboard PIA (Future: GIME graphics)
* Network cards: CocoIOr(with Axiom41C) or Bonobo 2.3, 2.4

## Network Packets

* Datagrams multiplexed into the TCP stream
* Header followed by Payload
* Header is 5 bytes: CNNPP (as in LOADM binary formats)
* C is 1-byte command
* N is 2-byte length (specifies payload size)
* P is 2-byte parameter (command-dependant)
* Max payload size is 64 bytes
* Packets are received Exactly Once (except after a crash)
* During Game play, all cocos receive all game packets in the same order

## Kernel Architecture

* MAIN + KERN + IRQ
* MAIN = Setup + Embark
* KERN = StartTask, NetworkHandler, interrupt Vectors, Fatal
* IRQ = Mono Clock + Wall Clock + TryNetworkReceive + BreakKeyScan + ChatShellKeyboard
* Network = Bonobo | Wiznet;  Send | TryReceive
* Also Alloc64, VDG, Console, Keyboard

## Process States

FG = foreground, the task running when not handling an IRQ

BG = background, only during IRQ handling

* ChatShell: FG = NetworkReceive; BG = Keyboard(30x/s), BreakKey(30x/s)
* GameHidden: FG = Game; BG = NetworkReceive(30x/s), Keyboard (20x/s), BreakKey (10x/s)
* GameVisible: FG = Game; BG = NetworkReceive(30x/s), BreakKey(30x/s)
* GameDebug: (future)

## Memory Map (based on coco2 with 16K RAM)

* $0000: Kernel Direct Page BSS
* $0080: Game Direct Page BSS (for game use)
* $0100: Coco1/2 Interrupt Relays
* $01FE downward: The Stack (game & kernel together)
* $0200: Eight 64-byte chunks of memory (e.g. deliver packets to game)
* $0400: VDG Text Console (for ChatShell, GameVisible, & GameDebug states)
* $0600: Kernel (initialized data, zeroed data, kernel code)
* ~1000: Kernel Setup code & data
* ~1000: Game (initialized data, zeroed data, game code)
* LowMark: Game Setup data
* LowMark: Begin available memory (unstructured, for game use)
* HighMark: End available memory
* Regions (common storage across game chaining)
* Screens (common storage across game chaining)
* $4000: Ram Limit

## Game Authoring

Divide code into setup() and loop() functions.

Stack is limited, so Global Variables are OKAY.

For a multi-coco game, the Game State must be replicated across Cocos.

Plan for low bandwidth (rough target: 64 bytes per second to transmit) &
high latency (updates once per second).

Who is the authority for each piece of Game State?

Packet arrival order can help: e.g. first to claim to kill a thing, last to move a thing)

## The Cow-Glider Game

Each player has a glider, like an Air Hockey Puck that keeps its momentum.

Players (one per coco) use arrows to apply forces Up, Down, Left, or Right to their glider.

Game is played on a 96x96 pixel 4-color screen.  
The Coco VGA graphics are in PMode1, which requires a 3K (12 256-byte page) screen buffer.
Since PMode1 is actually 128x96, a small, tall 32x96 strip on the right is used to
display scores.  The 96x96 game field is the large square part on the left.

There are also spots on the field called Cows.  If you glide slow enough on top
of a cow, you take the cow, and you score a point.


