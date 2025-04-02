#ifndef _NEKOT1_PUBLIC_H_
#define _NEKOT1_PUBLIC_H_

// 2025-03-13:  Proposed nekot1 `g` API for Nekot Gaming OS.

// In the following, "Game" is what we call the user programs
// or processes or applications or apps that this OS can run.

// The source for each game lives in a separate subdirectory
// of `games/` and the main program must be in a file named
// `game.c`.  In order for the compiler to do whole-program
// optimizations, your entire program must be included in
// `game.c`, even if it means adding #include statements
// to include other `.c` files!
// Place this repo `nekot-coco-microkernel` in your `coco-shelf`
// repo, and place the `coco-shelf` repo in your $HOME directory,
// for best chances of things working, for now.

// Notice that this "nekot1/public.h" is the ONLY #include file
// you need for Nekot.
// There is no <stdio.h> or <stdlib.h> or <memory.h>,
// although we may try to provide something for the most common cases
// (like we do for memcpy, memset, and strlen, and va_{list,start,arg}).

// Soon we hope to provide some optional libraries for game authors,
// to do common things like scan the keyboard for keypresses, or draw
// pixels in a framebuffer for various screen modes.
// Look in the ../lib8 directory for some preliminary libraries.

// We compile with the 6809 port of GCC using -std=gnu99, -Os,
// -fomit-frame-pointer, and f-whole-program.

// Notice that you will not have a `main()` function.
// Instead you must divide your main program into two parts like this:
//
//   * `void setup(void)` contains only setup code.
//
//   * `void loop(void)` contains the actual game action.  The memory used by
//        setup code and setup data will be made available for re-use,
//        if you do it this way.  [ TODO: finish this feature. ]
//
// (We are using the same concept and names as Arduino Sketches use.)
//
// Your loop function must call gReceiveCast64 frequently, like at least
// every 100ms.  And you must gFree64 a message you get from there.
// Or Nekot will hit a fatal error when it runs out of memory.

// Initialized global variables containing tables and data that
// are only needed during setup can be marked with the
// attribute `gSETUP_DATA` (put it before the `=`).
//
// Small global variables (things that are one or two bytes)
// that are not explicitly initialized (called .bss variables)
// are generally fine, but you can only have 128 bytes worth
// of them.  Larger variables that are not explicitly initialized
// can be marked with the attribute `gZEROED` to move them from
// the small .bss section to a section with more space.

/////////////////////
//
//  Fundamental Types and Definitions.

typedef unsigned char gbool;  // Recommended for true/false 1/0 values.
typedef unsigned char gbyte;  // Recommended for a 8-bit machine gbyte.  Unsigned!
typedef unsigned int gword;   // Recommended for a 16-bit machine gword.  Unsigned!

// We hope this union is both an efficient and an expressive word joiner/splitter.
typedef union gWordOrBytes {
    gword w;        // Access as a 2-Byte word, or
    gbyte b[2];     // access as two bytes (b[0] & b[1]).
    struct gHL { gbyte h, l; } hl;  // access as two bytes (hl.h & hl.l).
} gwob;

#define gTRUE ((gbool)1)
#define gFALSE ((gbool)0)
#define gNULL ((void*)0)
#define gALWAYS (gKern.always_true)  // For working around GCC infinite loop bugs.

#ifndef gCONST
#define gCONST const   // For variables the Kernel changes, but Games must not.
#endif

// You should not use things directly from "nekot1/friend.h";
// they are required for some "nekot1/public.h" macros to work.
#include "nekot1/friend.h"

#include <stdarg.h>  // You can write functions like `printf` with `...` args.

#define gPeek1(ADDR) (*(volatile gbyte*)(gword)(ADDR))
#define gPoke1(ADDR,VALUE) (*(volatile gbyte*)(gword)(ADDR) = (gbyte)(VALUE))

#define gPeek2(ADDR) (*(volatile gword*)(gword)(ADDR))
#define gPoke2(ADDR,VALUE) (*(volatile gword*)(gword)(ADDR) = (gword)(VALUE))

// These do a gPeek1, some bit manipulaton, and a gPoke1.
#define gPAND(ADDR, X) ((*(volatile gbyte*)(gword)(ADDR)) &= (gbyte)(X))
#define gPOR(ADDR, X) ((*(volatile gbyte*)(gword)(ADDR)) |= (gbyte)(X))
#define gPXOR(ADDR, X) ((*(volatile gbyte*)(gword)(ADDR)) ^= (gbyte)(X))

// If your ".bss" allocation of 128 bytes in Page 0 (the direct page)
// fills up, you can mark some of the global variable definitions with
// this attribute, to move those variables into a larger section.
#define gZEROED      __attribute__ ((section (".data.more")))

#define gAssert(COND) if (!(COND)) gFatal(__FILE__, __LINE__)

void gFatal(const char* s, gword value);

#define gDisableIrq() asm volatile("  orcc #$10")
#define gEnableIrq()   asm volatile("  andcc #^$10")
gbyte gIrqSaveAndDisable();
void gIrqRestore(gbyte cc_value);

////////////////////////
//  Pre-allocation

// Use these at the top of your main .c file
// to carve screens and regions out of high memory.
//
// Pre-allocating defines addresses for these
// before the compiler runs, so the compiler can
// generate more optimized code.
//
// Programs that start with some of these elements
// in common may chain values in memory from one
// to the other, for as many as are specified the same.

// gSCREEN(Name,NumPages) declares Name to be the address of
// a reserved framebuffer of the specified number of 256-byte pages.
#define gSCREEN(Name,NumPages)  // LIKE: extern gbyte Name[NumPages*256];

// gREGION(Name,Type) declares Name to be a global variable
// of the specified type.
#define gREGION(Name,Type)      // LIKE: extern Type Name;

// Example:
//
// gSCREEN(T, 2);   // T for Text, needs 2 pages (512 bytes).
// gSCREEN(G, 12);  // G for PMode1 Graphics, needs 12 pages (3K bytes).
// gREGION(Common, struct common);  // Common to all levels.
// gREGION(Maze, struct maze);     // Common to maze levels.

////////////////////////
//
//  64-gbyte Chunks

// gAlloc64 allocate a 64 gbyte Chunk of memory.
// Succeeds or returns gNULL.
void* gAlloc64();

// gFree64 frees a 64 gbyte Chunk that was allocated with gAlloc64().
// If ptr is gNULL, this function returns without doing anything.
void gFree64(void* ptr);

/////////////////////
//
//  GameCast Networking

// The network model that Games can directly use peer-to-peer
// is called GameCast.  It is like a Broadcast message, except
// that it only goes to other peers (other Cocos) in your same
// Game, in your same Game Shard.
//
// Message payloads can be 0 to 60 bytes long.
// They are sent with a 2-byte header, representing the
// player number of the sender, and a set of flag bits.
// The OS will fill in the player number of the sender.
// You should use zero the flags field (at lest until later when
// flags actually get defined).
// The final field `next` in the following struct is not
// transmitted or received.  It is available for making
// singly-linked lists of messages, if you need to.

struct gamecast {
    gbyte sender;
    gbyte flags;  // must be zero.
    gbyte payload[60];
    struct gamecast *next;
};

// gSendCast attempts to send a GameCast message of 0 to 60
// payload bytes to every active player in your game shard.
// The address you give does NOT have to be a Chunk allocated with gAlloc64.
// You will still own the memory, after you call gSendCast.
void gSendCast(const struct gamecast* ptr, gbyte payload_size);

// gReceiveCast64 attempts to receive a GameCast message sent by
// anyone in your game shard, including your own,
// that were set with gSendCast().  If no message has
// been received, the gNULL pointer is returned.
// If you need to know the length of the received
// message, that needs to be sent in the message.
// Bytes after the valid part of the payload that was sent
// will be arbitrary junk.
//
// IMPORTANT:  You should call gFree64() on the
// chunk when you are done with it.
struct gamecast* gReceiveCast64();

// If you can view the MCP logs, you can log to them.
// Don't log much!
void gNetworkLog(const char* s);

///////////////
//
//  Video Mode

// gTextScreen sets the VDG screen mode for game play to a Text Mode.
void gTextScreen(gbyte* screen_addr, gbyte colorset);

// gPMode1Screen sets the VDG screen mode for game play to PMode1 graphics.
void gPMode1Screen(gbyte* screen_addr, gbyte colorset);

// gModeScreen sets the VDG screen mode for game play to the given mode_code.
// TODO: document mode_code.
void gModeScreen(gbyte* screen_addr, gword mode_code);

/////////////////////
//
//  gKern Module

// Normal end of game.  Scores are valid and may be published
// by the kernel.
#define gGameOver(WHY)  xSendControlPacket('o', (gbyte*)(WHY), 64)

// Abnormal end of game.  Scores are invalid and will be ignored
// by the kernel.
// This is less drastic than calling gFatal(),
// because the kernel keeps running, but it also
// indicates something went wrong that should be fixed.
#define gGameAbort(WHY)  xSendControlPacket('a', (gbyte*)(WHY), 64)

// Replace the current game with the named game.
// This can be used to write different "levels" or
// interstitial screens as a chain of games.
// Carry scores forward to the new game.
// Common pre-allocated regions are also kept in memory.
#define gGameChain(NEXT_GAME_NAME)  xSendControlPacket('c', (gbyte*)(NEXT_GAME_NAME), 64)

// gPin(f) will pin down function f, so GCC doesn't erase it.
// Sometimes if you are using inline assembly language
// inside a bogus wrapper function, you need to tell GCC
// that the function is needed even if it isn't ever
// explicitly called.  Your `loop` function is a good place
// to use gPin.
#define gPin(THING)  gPoke2(0, &(THING))

// Global variables or data tables that are only used
// by setup code can be marked with the attribute
// gSETUP_DATA.  They will be freed when you call
// gAfterMain().
#define gSETUP_DATA   __attribute__ ((section (".data.startup")))

// The following gKern variables can be read by the game
// to find out what state the Kernel is in.
//
// A game should always see `in_game` is gTRUE!
// A game should always see `in_irq` is gFALSE!
//
// The important one is `focus_game`:  If `focus_game`
// is gTRUE, the game can scan the keyboard (but it must
// disable interrupts while doing so).
//
// If the game has an infinite loop (even one with a
// break, continue, or return in it) it is better to use
//
//     while (gALWAYS) { ... }
//
// rather than
//
//     while (gTRUE) { ... }
//
// due to bugs in our version of GCC.

struct kern {
    // TODO: memory curtains are not ready yet.
    // If your game wants to make raw allocations of memory
    // at runtime, the memory betweens these two curtains
    // is free.  If you take memory from the low part of
    // this region, increase the low_memory_curtain appropriately.
    // If you take memory from the high part of this region,
    // decrease the high_memory_curtain appropriately.
    // It is possible to write a library like malloc() and free()
    // that uses this memory.
    gword low_memory_curtain;
    gword high_memory_curtain;

    // When a game is active and focused, it owns and can scan the keyboard
    // (except for the BREAK key), and the game's screen
    // is being shown.   If a game is active but
    // focus_game is gFALSE, the game must ignore the
    // keyboard (not scan it!) and the game's screen is
    // not visible -- the Chat screen is shown, instead.
    gbool volatile focus_game;

    // gKern.always_true must always be set to 1.
    // Sampled by gALWAYS.
    gbool volatile gCONST always_true;

    // ---- The following fields are not needed by games: ----

    // A game is active.
    // From a game, this should always be seen as gTRUE.
    gbool volatile gCONST in_game;

    // We are currently handling a 60Hz Clock IRQ.
    // From a game, this should always be seen as gFALSE.
    gbool volatile gCONST in_irq;

    // To determine if setup allocated any low memory.
    // Only used by the kernel.
    gword gCONST old_low_memory_curtain_before_setup;

    // Internal.
    gword volatile saved_stack_pointer;
};
extern struct kern gKern;

/////////////////////
//
//  TODO: Scoring

// gMAX_PLAYERS is the maximum number of active players
// in a single game shard.
#define gMAX_PLAYERS 8

// TODO: Scoring does not work yet.
struct score {

// gScore.number_of_players is the current number of
// active players in the game.
       gCONST gbyte number_of_players;

// gScore.player tells you your player number
// (from 0 to gMAX_PLAYERS-1) if you are active in the game.
// If you are just a viewer, you are not an active player,
// and this variable will be 255.
       gCONST gbyte player;

// gScore.partials are contributions to scores from this coco.
// You change these to add or deduct points to a player.
       int partials[gMAX_PLAYERS];

// gScore.totals is the total score, calculated in the MCP.
// Read Only, set by the OS, the sum of all partial scores.
       gCONST int totals[gMAX_PLAYERS];

// Ignore these.  They are for internal use,
// to determine if the partials have changed.
       gCONST int old_partials[gMAX_PLAYERS];
};
extern struct score gScore;

/////////////////////
// Real Time

// Your game may watch these variables, and when
// they change, you can do some action that you want to
// do 60x, 10x, or 1x per second.  These variables
// will not match those on other cocos.  They are reset
// only when your coco boots, and as long as IRQs
// are never disabled for too long, they should
// reliably increment.  After about 18.2 hours,
// seconds wraps back to zero.

struct real {
    gbyte volatile ticks;  // Changes at 60Hz:  0 to 5
    gbyte volatile decis;  // Tenths of a second: 0 to 9
    gword volatile seconds;  // 0 to 65535
};
extern gCONST struct real gReal;

////////////////////////////
//
//  Wall Time

// If your game wants to display the date or time, it can use these
// variables.  These may not be accurate to the split second, but hopefully
// within a second or two.  It is possible that this time changes, even
// backwards, as it might be corrected asynchronously, and there is also
// no indication of daylight/summer time.  These are not problems we're
// trying to solve.

struct wall {
    gbyte volatile second; // 0 to 59
    gbyte volatile minute; // 0 to 59
    gbyte volatile hour;  // 0 to 23

    gbyte volatile day;   // 1 to 31
    gbyte volatile month; // 1 to 12
    gbyte volatile year2000;  // e.g. 25 means 2025
    gbyte volatile dow[4];  // e.g. Mon\0
    gbyte volatile moy[4];  // e.g. Jan\0

    // If hour rolls over from 23 to 0,
    // these values are copied to day, month, etc.
    // These are preset by the server, so the kernel does not
    // need to understand the Gregorian Calendar.
    gbyte next_day;
    gbyte next_month;
    gbyte next_year2000;
    gbyte next_dow[4];
    gbyte next_moy[4];
};
extern gCONST struct wall gWall;

///////////////////////////////////////////////////////
// 
//  Nekot Kernel Configuration

struct config {
    gword ram_limit;
    const char* net_type;
};
extern gCONST struct config gConfig;

typedef unsigned int size_t;
///////////////////////////////////////////////////////
// 
//  Standard Library Support

typedef unsigned int size_t;
void *memcpy(void* dest, const void* src, size_t n);
void *memset(void* s, int c, size_t n);
int strlen(const char* s);

// Nekot varients:
// Optimized for larger blocks.
void gMemcpy(void *dest, const void *src, gword count);
void gMemset(void* dest, gbyte value, gword count);

#endif // _NEKOT1_PUBLIC_H_
