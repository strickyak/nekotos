// kern.c

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

// If not in_game,
// NoGameTask() should be executed repeatedly
// in the foreground. 
void NoGameTask() {
    byte prev;
    while (Kern.always_true) {
        Console_Printf("*");
        if (Real.ticks != prev) {
            prev = Real.ticks;
            //?// asm volatile ("cwai #$00");
            SpinCWait();
        }
        CheckReceived();
        SpinNoGameTask();
    }
}

void NoGameMain() {
    while (true) {
        assert(!Kern.in_game);
        NoGameTask();
    }
}

// Only in Game Mode
void Network_Handler() {
    assert(Kern.in_game);
    CheckReceived();
}

void Kern_Init() {
    Kern.in_game = false;
    Kern.in_irq = false;
    Kern.always_true = true;
}
