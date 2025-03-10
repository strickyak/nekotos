#include "nekot1/private.h"

// kern.c

void FatalSpin(const char *why) {
    volatile gbyte* p = (volatile gbyte*) Cons;

    // Work around GCC infinite loop bug.
    while (Kern.always_true) {
        gPoke2(p, gPeek2(p) + 1);
    }
}

void Fatal(const char* why, gword arg) {
    INHIBIT_IRQ();

    SwitchToChatScreen();
    // Console_Printf("\nFATAL (%s, %d)", why, arg);
    PutStr("\nFATAL ");
    PutDec(arg);
    PutStr(": ");
    PutStr(why);
    FatalSpin(why);
}


// StartTask begins the given function entry,
// which must never return,
// as a foreground task,
// whether or not we were in the IRQ handler
// when we got here.
// Unless the entry is ChatTask, it starts 
// with in_game mode set to gTRUE.
void StartTask(gword entry) {
    if (!entry) {
        entry = (gword)ChatTask;
    }

    if (entry == (gword)ChatTask) {
        Kern.in_game = gFALSE;
        Kern.focus_game = gFALSE;
    } else {
        Kern.in_game = gTRUE;
        Kern.focus_game = gTRUE;
        // Until the game changes the display,
        // you get an Orange Console.
        gGameShowsTextScreen(Cons, COLORSET_ORANGE);
    }

    Kern.in_irq = gFALSE;
    asm volatile("\n"
        "  ldx   %0      \n"  // entry point
        "  lds   #$01FE  \n"  // Reset the stack
        "  andcc #^$50   \n"  // Allow interrupts.
        "  jmp   ,X      \n"  // There is no way back.
        : // outputs
        : "m" (entry) // inputs
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
    //PutChar(64 + (cc_value & 0x10));
    return cc_value;
}

void gIrqRestore(gbyte cc_value) {
    //PutChar(65 + (cc_value & 0x10));
    asm volatile("\n"
        "  ldb %0  \n"
        "  tfr b,cc  \n"
        :  // outputs
        : "m" (cc_value)  // inputs
        : "b", "cc"  // clobbers
    );
}

void gAfterMain3(gfunc after_main, gword* final, gword* final_startup) {
    // Prove that startup is no longer used.
    for (gword w = (gword)final; w < (gword)final_startup; w++) {
        gPoke1(w, 0x3F);
    }

    asm volatile("\n"
        "  ldx   %0      \n"  // entry point
        "  lds   #$01FE  \n"  // Reset the stack
        "  jsr   ,X      \n"  // There is no way back.
        "FOREVER:        \n"
        "  bra  FOREVER  \n"  // If after_main returns, get stuck.
        : // outputs
        : "m" (after_main) // inputs
    );
    // Never returns.
}

void ChatTask() {
    SwitchToChatScreen();

    while (Kern.always_true) {
        gAssert(!Kern.in_game);
        gAssert(!Kern.in_irq);

        CheckReceived();
        SpinChatTask();
    }
}

// Only in Game Mode
void Network_Handler() {
    gAssert(Kern.in_game);
    CheckReceived();
}

void Kern_Init() {
    Kern.in_game = gFALSE;
    Kern.focus_game = gFALSE;
    Kern.in_irq = gFALSE;
    Kern.always_true = gTRUE;
}
