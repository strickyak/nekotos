#ifndef _KERNEL_KERN_H_
#define _KERNEL_KERN_H_

// kern.h

#define GAME_BSS_BEGIN 0x0080
#define GAME_BSS_LIMIT 0x0100

extern gword volatile SavedStackPointer;  // in IRQ Handler

void Kern_Init();
void ChatTask();
void Network_Handler();
void gFatalSWI1();
void gFatalSWI2();
void gFatalSWI3();
void gFatalFIRQ();

void WrapNMI();
void HandleNMI();

void StartTask(gword entry);
void xAfterSetup(gfunc loop, gword* final_, gword* final_startup);

gbyte IrqSaveAndDisable();
void IrqRestore(gbyte cc_value);

/*
///////////////////////////////////////
//
//  New Memory Map 2024-04-02

256B
Page Purpose
---  -------
0   Direct Page globals variables (Kernel $00-$7F and Game $80-$FF?)
1   Stack (Kernel and Game and IRQ) $1FE downward (Keep $1FE as Canary)
2,3 Eight 64B chunks for Alloc64(), Free64().
4,5 Console VDG Text Display
6   Kernel starts: ".data", ".text", ".text.startup"

Kernel's ".text.startup" can be wiped after main calls out,
never to return.

So Game starts at the same place as Kernel's ".text.startup":
    ".data", ".text", ".text.startup"

63  End page working backward:
    Declared SCREEN memory.
    Declared COMMON regions.
    High Water Mark
    Unused (available) memory
    Low Water Mark
    Game.
*/

#endif  // _KERNEL_KERN_H_
