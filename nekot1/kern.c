#include "nekot1/private.h"

// kern.c

void FatalSpin(const char *why) {
    volatile gbyte* p = (volatile gbyte*) Cons;

    // Work around GCC infinite loop bug.
    while (gALWAYS) {
        gPoke2(p, gPeek2(p) + 1);
    }
}

void gFatal(const char* why, gword arg) {
    gDisableIrq();
    // Reify things
    gPoke2(0, &gFatal);
    gPoke2(0, &why);
    gPoke2(2, &arg);

    NowSwitchToChatScreen();
    PutStr("\nFATAL ");
    PutDec(arg);
    PutStr(": ");
    PutStr(why);
    PutStr("\n$");

    const gbyte* p = (const gbyte*)gPeek2(4);
    PutHex( (gword)p );
    PutStr(": ");
    for (gbyte i = 0; i < 32; i++) {
        PutHex(p[i]);
        if ((i&3)==3) PutChar(',');
        if ((i&15)==15) PutChar(';');
        PutChar(' ');
    }
    PutChar('$');
    FatalSpin(why);
}

void gFatalSWI1() {
    asm volatile("sts %0" :: "m" (gKern.saved_stack_pointer));
    gFatal("SWI", gKern.saved_stack_pointer);
}
void gFatalSWI2() {
    asm volatile("sts %0" :: "m" (gKern.saved_stack_pointer));
    gFatal("SWI2", gKern.saved_stack_pointer);
}
void gFatalSWI3() {
    asm volatile("sts %0" :: "m" (gKern.saved_stack_pointer));
    gFatal("SWI3", gKern.saved_stack_pointer);
}
void gFatalNMI() {
    asm volatile("sts %0" :: "m" (gKern.saved_stack_pointer));
    gFatal("NMI", gKern.saved_stack_pointer);
}
void gFatalFIRQ() {
    asm volatile("sts %0" :: "m" (gKern.saved_stack_pointer));
    gFatal("FIRQ", gKern.saved_stack_pointer);
}

// StartTask begins the given function entry,
// which must never return,
// as a foreground task,
// whether or not we were in the IRQ handler
// when we got here.
// Unless the entry is ChatTask, it starts 
// with in_game mode set to gTRUE.
void StartTask(gword entry) {
    gDisableIrq();

    if (!entry) {
        entry = (gword)ChatTask;
    }

    // Zero the Game's BSS.
    memset_words(GAME_BSS_BEGIN, 0, (GAME_BSS_LIMIT-GAME_BSS_BEGIN)>>1);

    if (entry == (gword)ChatTask) {
        gKern.in_game = gFALSE;
        gKern.focus_game = gFALSE;
    } else {
        gKern.in_game = gTRUE;
        gKern.focus_game = gTRUE;
        // Until the game changes the display,
        // you get an Orange Console.
        gTextScreen(Cons, COLORSET_ORANGE);
    }

    // Set SWI Traps at start of memory (scratchpad).
    memset_words(0x0000u, 0x3F3F, 8);
    // TODO: between app's low water and high water.
#if 0
    // Breaks on 16K coco2:
    memset_words(0x4000u, 0x3F3F, (0xFE00u - 0x4000u)>>1);
#endif

    gKern.in_irq = gFALSE;
    asm volatile("\n"
        "  ldx   %0      \n"  // entry point
        "  lds   #$01FE  \n"  // Reset the stack
        "  andcc #^$50   \n"  // Allow interrupts.
        "  jmp   ,X      \n"  // There is no way back.
        : // outputs
        : "m" (entry) // input %0
    );
    // Never returns.
}

gbyte gIrqSaveAndDisable() {
    gbyte cc_value;
    asm volatile("\n"
        "  tfr cc,b  \n"
        "  stb %0  \n"
        "  orcc #$10  \n"
        : "=m" (cc_value)  // outputs
        :  // inputs
        : "b"  // Clobbers B
    );
    return cc_value;
}

void gIrqRestore(gbyte cc_value) {
    asm volatile("\n"
        "  ldb %0  \n"
        "  tfr b,cc  \n"
        :  // outputs
        : "m" (cc_value)  // inputs
        : "b", "cc"  // clobbers
    );
}

void xAfterSetup(gfunc loop, gword* final_, gword* final_startup) {
    // Prove that setup is no longer used.
    for (gword w = 2+(gword)final_; w < (gword)final_startup; w++) {
        gPoke1(w, 0x3F);  // a bunch of SWI traps.
    }

    asm volatile("\n"
        "  ldx   %0           \n"  // get parameter `loop` before resetting the stack.
        "  lds   #$01FE       \n"  // Reset the stack!
        "  pshs  X            \n"  // Save X
        "LOOP_FOREVER:        \n"
        "  ldx   ,S           \n"  // Restore X
        "  jsr   ,X           \n"  // Call loop,
        "  bra  LOOP_FOREVER  \n"  // repeatedly.
        : // outputs
        : "m" (loop) // inputs
    );
    // Never returns.
}

void ChatTask() {
    // debugging blocks
    // volatile gbyte* p = (volatile gbyte*)Cons;
    // while (gKern.always_true) { p[33]++; }
    // while (!gKern.always_true) { p[35]++; }
    
    NowSwitchToChatScreen();

    while (gALWAYS) {
        gAssert(!gKern.in_game);
        gAssert(!gKern.in_irq);

        CheckReceived();
        SpinChatTask();
    }
}

// Only in Game Mode
void Network_Handler() {
    gAssert(gKern.in_game);
    CheckReceived();
}

void Kern_Init() {
    gKern.in_game = gFALSE;
    gKern.focus_game = gFALSE;
    gKern.in_irq = gFALSE;
    gKern.always_true = gTRUE;
}
