#ifndef _N1_KERN_H_
#define _N1_KERN_H_

// kern.h

struct kern {
    bool volatile always_true;
    bool volatile in_game;
    bool volatile in_irq;
} Kern;

void NoGameTask();
void Network_Handler();

// If entry is 0, it is the no-game task.
void StartTask(word entry);

// Shouldn't these be public?
byte IrqSaveAndDisable();
void IrqRestore(byte cc_value);

/*
///////////////////////////////////////
//
//  New Memory Map 2024-02-21

256B
Page Purpose
---  -------
0   Direct Page globals variables (Kernel $00-$7F and Game $80-$FF?)
1   Stack (Kernel and Game and IRQ) $1FE downward (Keep $1FE as Canary)
2,3 Console VDG Text Display
4   Four 64B chunks for the Kernel to use.
5   Kernel starts: ".data", ".text", ".text.startup"

Kernel's ".text.startup" can be wiped after main calls out,
never to return.

So Game starts at the same place as Kernel's ".text.startup":
    ".data", ".text", ".text.startup"

63  End page working backward:  Declared SCREEN memory.
    64B Chunks for alloc/free.
    (Before Game's .text.startup is over, chunks are
    available to end of Game.  After .text.startup is over,
    chunks are available to end of ".text".

*/

#endif // _N1_KERN_H_
