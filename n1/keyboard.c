#include "n1/private.h"

static bool ScanReturnChanged(byte* p, byte* prev) {
    const word out_port = Pia0PortB;
    const word in_port = Pia0PortA;
    bool z = false;

    byte sense = 0x01;
    while (sense) {
        Poke1(out_port, ~(byte)sense);
        *p = 0x7F & ~Peek1(in_port);
        if (*p++ != *prev++) {
            z = true; // found a difference.
        }
        sense <<= 1;
    }
    SpinKeyboardScan();
    return z;
}

static void SendKeyboardPacket(byte* p) {
    struct quint q = {NEKOT_KEYSCAN, 8, 0};
    WizSend( (byte*) &q, sizeof q );
    WizSend( p, 8 );
}

void KeyboardHandler() {
    if (Kern.focus_game) return;
    assert(Kern.in_irq);

    byte current = Keyboard.current_matrix;
    byte other = !current;

    bool changed = ScanReturnChanged(Keyboard.matrix[current], Keyboard.matrix[other]);
    if (changed) {
        SendKeyboardPacket(Keyboard.matrix[current]);
    }

    Keyboard.current_matrix = other;
}
