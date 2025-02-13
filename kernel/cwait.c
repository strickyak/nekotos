// This is a "Null Game" that does nothing.
void CWait(void) {
    ALLOW_IRQ();

    // Go into a loop where we just wait on interrupts.
    asm volatile ("   \n"
        "CWait:       \n"
        "  cwai  #$00 \n"  // wait for any interrupt.
        "  inc  $0200 \n"  // First byte of Kernel Console.
        "  inc  $0400 \n"  // First byte of Game Display.
        "  jmp CWait  \n"
        );
}
