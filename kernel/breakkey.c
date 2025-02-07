void DoBreak(void) {
    // TODO
}

// Break_Handler is called on interrupt.
void Breakkey_Handler(void) {
    const byte probe = ~(byte)0x04;
    Poke1(Pia0PortB, probe);  // Place the keyboard probe signal for BREAK
    byte sense = Peek1(Pia0PortA);

    if ((sense & 0x40) == 0) {  // Check the sense bit for BREAK
        DoBreak();
    }
}
