static void Scan(byte* p) {
    const word out_port = Pia0PortB;
    const word in_port = Pia0PortA;

    Poke1(out_port, ~(byte)0x01);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x02);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x04);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x08);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x10);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x20);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x40);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x80);
    *p++ = 0x7F & ~Peek1(in_port);
}

// Did the keyboard change from the previous to the current matrix?
static bool Changed(byte* current, byte* prev) {
    // Convert to word* to use a stride of 2.
    word* a = (word*)current;
    word* b = (word*)prev;
    // Now four times with a stride of 2, for 8 bytes.
    if (*a++ != *b++) return true;
    if (*a++ != *b++) return true;
    if (*a++ != *b++) return true;
    if (*a++ != *b++) return true;
    return false; // No change.
}

static void SendKeyboardPacket(byte* p) {
}

void Keyboard_Handler() {
    byte current = Keyboard.current_matrix;
    byte other = !current;

    Scan(Keyboard.matrix[current]);
    bool changed = Changed(Keyboard.matrix[current], Keyboard.matrix[other]);
    if (changed) SendKeyboardPacket(Keyboard.matrix[current]);

    Keyboard.current_matrix = other;
}
