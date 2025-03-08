#ifndef _N1_PUBLIC_H_
#define _N1_PUBLIC_H_

// Proposed "N1" API for Nekot Gaming OS.

// In the following, remember that "Game" is what we call the 
// user programs or processes or applications or apps that this OS can run.

/////////////////////
//
//  Fundamental Types and Definitions.

typedef unsigned char bool;
typedef unsigned char byte;  // Best type for a 8-bit machine byte.
typedef unsigned int word;  // Best type for a 16-bit machine word.
typedef void (*func)(void);
typedef union wordorbytes {
    word w;
    byte b[2];
} wob;

#define true ((bool)1)
#define false ((bool)0)
#define NULL ((void*)0)

#ifndef CONST
#define CONST const   // For variables the Kernel changes, but Games must not.
#endif

#define Peek1(ADDR) (*(volatile byte*)(word)(ADDR))
#define Poke1(ADDR,VALUE) (*(volatile byte*)(word)(ADDR) = (byte)(VALUE))

#define Peek2(ADDR) (*(volatile word*)(word)(ADDR))
#define Poke2(ADDR,VALUE) (*(volatile word*)(word)(ADDR) = (word)(VALUE))

// These do a Peek1, some bit manipulaton, and a Poke1.
#define PAND(ADDR, X) ((*(volatile byte*)(word)(ADDR)) &= (byte)(X))
#define POR(ADDR, X) ((*(volatile byte*)(word)(ADDR)) |= (byte)(X))
#define PXOR(ADDR, X) ((*(volatile byte*)(word)(ADDR)) ^= (byte)(X))

// If your ".bss" allocation of 128 bytes in Page 0 (the direct page)
// fills up, you can mark some of the global variable definitions with
// this attribute, to move those variables into a larger section.
#define MORE_DATA      __attribute__ ((section (".data.more")))

#define assert(COND) if (!(COND)) Fatal(__FILE__, __LINE__)

void Fatal(const char* s, word value);

#define INHIBIT_IRQ() asm volatile("  orcc #$10")
#define ALLOW_IRQ()   asm volatile("  andcc #^$10")
byte N1IrqSaveAndDisable();
void N1IrqRestore(byte cc_value);

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

#ifndef N1_DEFINE_SCREEN
#define N1_DEFINE_SCREEN(Name,NumPages)       extern byte Name[NumPages*256];
#endif

#ifndef N1_DEFINE_REGION
#define N1_DEFINE_REGION(Type,Name)    extern Type Name;
#endif

// Example:
//
// N1_DEFINE_SCREEN(T, 2);   // T for Text, needs 2 pages (512 bytes).
// N1_DEFINE_SCREEN(G, 12);  // G for PMode1 Graphics, needs 12 pages (3K bytes).
// N1_DEFINE_REGION(struct common, Common, 44);  // Common to all levels.
// N1_DEFINE_REGION(struct maze, Maze, 106);     // Common to maze levels.

////////////////////////
//
//  64-byte Chunks

// N1Alloc64 allocate a 64 byte Chunk of memory.
// Succeeds or calls Fatal().
byte* N1Alloc64();

// N1Free64 frees a 64 byte Chunk that was allocated with N1Alloc64().
// If ptr is NULL, this function returns without doing anything.
void N1Free64(byte* ptr);

/////////////////////
//
//  Networking

// In the following, messages can be 1 to 64 bytes long.
// The very first byte is reserved by the OS, and all the rest
// are available to the game.  The OS fills in the
// player number of the sender in the first byte.
// The operating system will call N1Free64 when it
// has been sent.

// N1Send64 attempts to send a "multicast" message of 1 to 64 bytes
// to every active player in your game shard.
// It succeeds or it calls Fatal().
void N1Send64(byte* ptr, byte size); 

// N1Receive64 attempts to receive a "multicast" message sent by 
// anyone in your game shard, including your own,
// that were set with N1Send().  If no message has
// been received, the NULL pointer is returned.
// If you need to know the length of the received
// message, that needs to be sent in the "fixed"
// portion at the front of the message, perhaps as
// the second byte.  You should call N1Free() on the
// chunk when you are done with it.
byte* N1Receive64();

// If you can view the MCP logs, you can log to them.
// Don't log much!
void N1NetworkLog(const char* s);

///////////////
//
//  Video Mode

// N1GameShowsTextScreen sets the VDG screen mode for game play to a Text Mode.
void N1GameShowsTextScreen(byte* screen_addr, byte colorset);

// N1GameShowsPMode1Screen sets the VDG screen mode for game play to PMode1 graphics.
void N1GameShowsPMode1Screen(byte* screen_addr, byte colorset);

// N1ModeForGame sets the VDG screen mode for game play to the given mode_code.
// TODO: document mode_code.
void N1ModeForGame(byte* screen_addr, word mode_code);

// GetConsoleTextModeAddress return the starting address
// of the kernel's Text Console, which is always 512 bytes,
// in the ordinary VDG Text mode.  The first 32 bytes
// are the "top bar" that can might be useful for
// realtime debugging markings.  Production games
// shouldn't need this.
inline byte* GetConsoleTextModeAddress() {
    return (byte*)0x0200;
}

/////////////////////
//
//  Scoring

// N1_MAX_PLAYERS is the maximum number of active players
// in a single game shard.
#define N1_MAX_PLAYERS 8

// N1NumberOfPlayers is the current number of
// active players in the game.
extern CONST byte N1NumberOfPlayers;

// YN1ThisPlayerNumber tells you your player number
// (from 0 to N1_MAX_PLAYERS-1) if you are active in the game.
// If you are just a viewer, you are not an active player,
// and this variable will be 255.
extern CONST byte N1ThisPlayerNumber;

// You change these to add or deduct points to a player.
extern int N1PartialScores[N1_MAX_PLAYERS];

// Read Only, set by the OS, the sum of all Partial Scores.
extern CONST int N1TotalScores[N1_MAX_PLAYERS];

/////////////////////
//
//  Kern Module

// Normal end of game.  Scores are valid and may be published
// by the kernel.
void N1GameOver(char *why);

// Abnormal end of game.  Scores are invalid and will be ignored
// by the kernel.
// This is less drastic than calling Fatal(),
// because the kernel keeps running, but it also
// indicates something went wrong that should be fixed.
void N1GameAbort(char *why);

// Replace the current game with the named game.
// This can be used to write different "levels" or
// interstitial screens as a chain of games.
// Carry scores forward to the new game.
// Common pre-allocated regions are also kept in memory.
void N1GameChain(char* next_game_name);

// N1AfterMain does not end the game -- it just ends the startup code,
// and frees the startup code in memory, making that memory available.
//
// If you call this at the end of your main function,
// it will exit the main function and enter the function f.
// The idea is that you put all your startup initialization
// code in main, and the memory for the startup code is
// freed up when the startup is done.  f points to the
// function where the non-startup code continues.
#define N1AfterMain(after_main) N1AfterMain3((after_main), &_n1pre_final, &_n1pre_final_startup)
void N1AfterMain3(func after_main, word* final, word* final_startup);

//X #define N1_STARTER()                                                        \
//X     word _n1pre_final __attribute__ ((section (".final")));                 \
//X     word _n1pre_final_startup __attribute__ ((section (".final.startup")));

// Global variables or data tables that are only used
// by startup code can be marked with the attribute
// STARTUP_DATA.  They will be freed when you call
// N1AfterMain().
#define STARTUP_DATA   __attribute__ ((section (".data.startup")))

// The following Kern variables can be read by the game
// to find out what state the Kernel is in.
//
// A game should always see `in_game` is true!
// A game should always see `in_irq` is false!
//
// The important one is `focus_game`:  If `focus_game`
// is true, the game can scan the keyboard (but it must
// disable interrupts while doing so). 
//
// If the game has an infinite loop (say, as the
// outer game loop) it is better to use
//
//     while (Kern.always_true) { ... }
//
// rather than
//
//     while (true) { ... }
//
// due to bugs in GCC.

extern CONST struct kern {
    // For GCC bug workaround.  Must always be true.
    bool volatile always_true;

    // A game is active and has the foreground task.
    bool volatile in_game;

    // We are currently handling a 60Hz Clock IRQ.
    bool volatile in_irq;

    // The active game also owns and can scan the keyboard
    // (except for the BREAK key), and the game's screen
    // is being shown.   If a game is active but
    // focus_game is false, the game must ignore the
    // keyboard (not scan it!) and the game's screen is
    // not visible -- the Chat screen is shown, instead.
    bool volatile focus_game;
} Kern;

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

extern CONST struct real {
    byte volatile ticks;  // Changes at 60Hz:  0 to 5
    byte volatile decis;  // Tenths of a second: 0 to 9
    word volatile seconds;  // 0 to 65535
} N1Real;  // Instance is named Real.

////////////////////////////
//
//  Wall Time

// If your game wants to display the date or time, it can use these
// variables.  These may not be accurate to the split second, but hopefully
// within a second or two.  It is possible that this time changes, even
// backwards, as it might be corrected asynchronously, and there is also
// no indication of daylight/summer time.  These are not problems we're
// trying to solve.

extern CONST struct wall {
    byte volatile second; // 0 to 59
    byte volatile minute; // 0 to 59
    byte volatile hour;  // 0 to 23

    byte volatile day;   // 1 to 31
    byte volatile month; // 1 to 12
    byte volatile year2000;  // e.g. 25 means 2025
    byte volatile dow[4];  // e.g. Mon\0
    byte volatile moy[4];  // e.g. Jan\0

    // If hour rolls over from 23 to 0,
    // these values are copied to day, month, etc.
    // These are preset by the server, so the kernel does not
    // need to understand the Gregorian Calendar.
    byte next_day;
    byte next_month;
    byte next_year2000;
    byte next_dow[4];
    byte next_moy[4];
} N1Wall; // Instance is named Wall.

#endif // _N1_PUBLIC_H_
