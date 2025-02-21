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

struct QuintAndKeyboardPacket {
    struct quint q;
    byte matrix[8];
};
static void SendKeyboardPacket(byte* p) {
    struct QuintAndKeyboardPacket qak = {
        {69, 8, 0},
        {0},
    };
    MemCopy(qak.matrix, p, 8);
    WizSend( (byte*) &qak, sizeof qak );
}

void KeyboardHandler() {
    if (Kern.in_game) return; // TODO: different Irq_Schedule.

    byte current = Keyboard.current_matrix;
    byte other = !current;

    Scan(Keyboard.matrix[current]);
    bool changed = Changed(Keyboard.matrix[current], Keyboard.matrix[other]);
    if (changed) SendKeyboardPacket(Keyboard.matrix[current]);

    Keyboard.current_matrix = other;
}
