void DoBreak(void) {
    // TODO
}

// Break_Handler is called on interrupt.
void Breakkey_Handler(void) {
    // Place the keyboard probe signal for BREAK
    const byte probe = ~(byte)0x04;
    Poke1(Pia0PortB, probe);

    // Read the sense port and check the bit.
    byte sense = Peek1(Pia0PortA);
    if ((sense & 0x40) == 0) {  // BREAK?
        DoBreak();
        SpinBreakkey();
    }
}
