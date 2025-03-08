#include "n1/private.h"

#if 0
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
#endif

static bool ScanReturnChanged(byte* p, byte* prev) {
    const word out_port = Pia0PortB;
    const word in_port = Pia0PortA;
    bool z = false;
    Console_Printf("S");

    byte sense = 0x01;
    while (sense) {
        Poke1(out_port, ~(byte)sense);
        *p = 0x7F & ~Peek1(in_port);
        if (*p++ != *prev++) {
            z = true; // found a difference.
        }
        sense <<= 1;
    }
    return z;
}

static void SendKeyboardPacket(byte* p) {
    struct quint q = {NEKOT_KEYSCAN, 8, 0};
    WizSend( (byte*) &q, sizeof q );
    WizSend( p, 8 );
}

void KeyboardHandler() {
    if (Kern.focus_game) return; // TODO: different Irq_Schedule.
    assert(Kern.in_irq);

    byte current = Keyboard.current_matrix;
    byte other = !current;

    Console_Printf("K");
    bool changed = ScanReturnChanged(Keyboard.matrix[current], Keyboard.matrix[other]);
    if (changed) {
        Console_Printf("C");
        SendKeyboardPacket(Keyboard.matrix[current]);
    }

    Keyboard.current_matrix = other;
}
