#ifndef _NEKOT1_KERN_H_
#define _NEKOT1_KERN_H_

// kern.h

struct kern gKern;

void Kern_Init();
void ChatTask();
void Network_Handler();

void StartTask(gword entry);

// Shouldn't these be public?
gbyte IrqSaveAndDisable();
void IrqRestore(gbyte cc_value);

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

#endif // _NEKOT1_KERN_H_
