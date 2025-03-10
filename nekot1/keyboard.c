#include "nekot1/private.h"

static gbool ScanReturnChanged(gbyte* p, gbyte* prev) {
    const word out_port = Pia0PortB;
    const word in_port = Pia0PortA;
    gbool z = false;

    gbyte sense = 0x01;
    while (sense) {
        Poke1(out_port, ~(gbyte)sense);
        *p = 0x7F & ~Peek1(in_port);
        if (*p++ != *prev++) {
            z = true; // found a difference.
        }
        sense <<= 1;
    }
    SpinKeyboardScan();
    return z;
}

static void SendKeyboardPacket(gbyte* p) {
    struct quint q = {NEKOT_KEYSCAN, 8, 0};
    WizSend( (gbyte*) &q, sizeof q );
    WizSend( p, 8 );
}

void KeyboardHandler() {
    if (Kern.focus_game) return;
    assert(Kern.in_irq);

    gbyte current = Keyboard.current_matrix;
    gbyte other = !current;

    gbool changed = ScanReturnChanged(Keyboard.matrix[current], Keyboard.matrix[other]);
    if (changed) {
        SendKeyboardPacket(Keyboard.matrix[current]);
    }

    Keyboard.current_matrix = other;
}
