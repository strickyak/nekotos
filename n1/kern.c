#include "n1/private.h"

// kern.c

void FatalSpin(const char *why) {
    volatile byte* p = (volatile byte*) Cons;

    // Work around GCC infinite loop bug.
    while (Kern.always_true) {
        Poke2(p, Peek2(p) + 1);
    }
}

void Fatal(const char* why, word arg) {
    INHIBIT_IRQ();

    SwitchToChatScreen();
    Console_Printf("\nFATAL (%s, %x, %u, %d.)", why, arg, arg, arg);
    FatalSpin(why);
}


// StartTask begins the given function entry,
// which must never return,
// as a foreground task,
// whether or not we were in the IRQ handler
// when we got here.
// Unless the entry is ChatTask, it starts 
// with in_game mode set to true.
void StartTask(word entry) {
    if (entry == (word)ChatTask) {
        Kern.in_game = false;
    } else {
        Kern.in_game = true;
    }

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

byte IrqSaveAndDisable() {
    byte cc_value;
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

void IrqRestore(byte cc_value) {
    asm volatile("\n"
        "  ldb %0  \n"
        "  tfr b,cc  \n"
        :  // outputs
        : "m" (cc_value)  // inputs
        : "b", "cc"  // clobbers
    );
}


void ChatTask() {
    SwitchToChatScreen();
    while (Kern.always_true) {
        assert(!Kern.in_game);

        CheckReceived();
        SpinChatTask();
    }
}

// Only in Game Mode
void Network_Handler() {
    assert(Kern.in_game);
    CheckReceived();
}

void Kern_Init() {
    Kern.in_game = false;
    Kern.focus_game = false;
    Kern.in_irq = false;
    Kern.always_true = true;
}
