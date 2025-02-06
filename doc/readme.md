# The NEKOT COCO Microkernel API for Games

NEKOT is a multiplayer gaming microkernel for Tandy / Radio Shack Color
Computers with high-speed networking.

## Design Goals:

*   Make it easy to write multiplayer networked games, where each player
is on one coco, and the cocos communicate over the internet (or intranet
or LAN) to work together to play the game.

*   Provide communication with the players before and after a game,
and a repository of games that are available under the OS.

*   'Game' referes to the type of App or Program that this OS can launch.
'Games' can also be things that are not really competitive games, like
Clock and Weather screens, Graphics Demos, or sound and video players.

*   The OS has two modes:  Chat and Game.  Any time a Game is not
being played, a text screen shows up for Chat messages.  The Chat mode
is also used to choose and launch games.

*   The OS can keep scores on a scoreboard and leader board.

*   The OS can terminate games when their time is up or they are over.

*   Use "Least common denominator" hardware requirements, running on any Coco1,
Coco2, or Coco3 with at least 16K of RAM.  (Probably Dragons as well.)

*   Games can be written in C or Assembly (and maybe other languages).
The Microkernel API will be described in C.

## Static Declarations for Games:

These C macros are used by scripts that generate the layout of the
compiled games.  Place them at the top of the C file,
in the global scope.

`DECLARE_GAME(NameOfGame, "VersionString")`:
Each game needs a main module with this declaration.

`DECLARE_FB(FrameBufferName, num256BytePages)`
In order to have video screens, the game must allocate frame buffers.
A text fb needs 2 pages.   A Pmode1 fb needs 12 pages.

`DECLARE_OVERLAY(NameOfOverlay, "VersionString")`:
(Future):  The game can chain to overlays, which are separately
compiled modules, to create longer, more complex games and levels.


## The Microkernel API for Games:

### Video Modes

The microkernel handles changing video modes, so it can suspend
and resume games.

`GameTextMode(fb_addr, css)`:  Switches the game to use text
(and semigraphics) mode at the given address, and with color set
either 0 (green) or 1 (orange).

`GamePMode1(fb_addr, css)`:  Switches the game to use PMode1,
with frame buffer at the given address, and with color set
either 0 or 1.

### Network Messages

The microkernel can transmit broadcast messages to and from
other members of your game shard.  Plan on each machine sending
at most one 32-byte packet per second.  There may be up to 8 players.

Packets can contain up to 32 bytes of data.  The first byte is the
player number.  The second byte is the length of the payload, 0 to 30.
The remaining bytes are the payload.  When you send a packet,
the OS will put your player number in the first byte for you.

Multiplayer games should be designed in a way that each node is
authoritative for calculating the game play for its own disjoint
portion of the game.  For instance, in the spacewar game, each node
calculates the velocity and position of both its rocket and its
bullet, and calculates collisions with that bullet and any player.
Once you have your own and all the packets from other players,
that should determine the entire state of the game, so all cocos
can display the same game.  Any other info should be "common knowledge"
across all nodes, compiled into all copies of the game,
such as the shape of a track, or the location of fixed barriers.

`byte GameNumberPlayers;`  // Set by the OS to how many players there are.  1 for Solitaire Games.  Up to 8 for multiplayer games.

`byte GamePlayerNumber;`  // Set by the OS to your Player Number, (0 to GameNumberPlayers-1).

`bool GamePacketSendReady;`  // The OS sets it true if a packet can be sent.

`GamePacketSend(byte* pointerToPacket)'

`bool GamePacketRecvReady;`  // The OS sets it true if a packet is ready to be received.

`GamePacketRecv(byte* pointerTo32ByteBuffer)'

### Score Points

The OS automatically communicates partial scores with your messages.
Each node should keep points for disjoint portions of the game in Partial Scores.
The OS computes Total Scores for you.  Only use as many items in these arrays
as there are players in your game shard.

`char *GamePlayerNames[8];`  // The OS sets the three-letter player names as strings here.  Unused slots are NULL pointers (value zero).

`int GamePartialScores[8];`  // You write positive or negative numbers here to add or deduct points from each player's score.

`int GameTotalScores[8];'  // The OS sums up all the Partial Scores on all nodes, to give you Total Scores.

### End of Game

At the end of the game, you may call GameOver to terminate the game.
If there is a serious problem like an assertion fails, you can call GameAbort to abort the game.

If any coco in your shard calls GameOver or GameAbort, your game is also terminated.
The `why` message is displayed on all cocos.

`GameOver(char* why)`  // Normal termination.

`GameAbort(char* why)`  // Abnormal termination.

### Logging and Debugging

`GameLog(char* message)`  // Log the message in the networked chat

`GameDebug(char* message)`  // Log the message in the local coco's chat

### Chaining (future)

In the future, you can split a game into smaller independant pieces that
play in sequence, one at a time.  Call GameChain instead of GameOver or GameAbort,
so the coco will go into that game insted of going into Chat mode.

`GameChain(char* name)`  // Specify an overlay in this game, or another game to play next

`struct common Common`  // A variable named `Common`, of type `struct common`, will be shared
across all overlays of a game.  Define this struct in a file named `common.h`.
