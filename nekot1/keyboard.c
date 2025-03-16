#include "nekot1/private.h"

static gbool ScanReturnChanged(gbyte* p, gbyte* prev) {
    const gword out_port = Pia0PortB;
    const gword in_port = Pia0PortA;
    gbool z = gFALSE;

    gbyte sense = 0x01;
    while (sense) {
        gPoke1(out_port, ~(gbyte)sense);
        *p = 0x7F & ~gPeek1(in_port);
        if (*p++ != *prev++) {
            z = gTRUE; // found a difference.
        }
        sense <<= 1;
    }
    SpinKeyboardScan();
    return z;
}

static void SendKeyboardPacket(gbyte* p) {
    struct quint q = {NEKOT_KEYSCAN, 8, 0};
    NET_Send( (gbyte*) &q, sizeof q );
    NET_Send( p, 8 );
}

void KeyboardHandler() {
    if (gKern.focus_game) return;
    gAssert(gKern.in_irq);

    gbyte current = Keyboard.current_matrix;
    gbyte other = !current;

    gbool changed = ScanReturnChanged(Keyboard.matrix[current], Keyboard.matrix[other]);
    if (changed) {
        SendKeyboardPacket(Keyboard.matrix[current]);
    }

    Keyboard.current_matrix = other;
}
