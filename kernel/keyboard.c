#include "keyboard.inc"
struct keyboard Keyboard;

void kbd_matrix_scanner(byte* matrix) {
    volatile byte* const out_port = (byte*)0xFF02;
    volatile byte* const in_port = (byte*)0xFF00;

    *out_port = 0;
    byte anything = 0x7F & ~(*in_port);
    if (!anything) {
        goto NOT_FOUND;
    }

    {
        byte i = 8;
        byte probe = 1;
        byte* p = matrix;
        do {
            *out_port = ~(byte)(probe);
            *p++ = 0x7F & ~(*in_port);
            probe <<= 1;
        } while (i--);
    }

NOT_FOUND:
    memset(matrix, 0, 8);
}
bool kbd_scanner_changed(byte* matrix, byte* prev) {
    kbd_matrix_scanner(matrix);
    uint* m = (uint*)matrix;
    uint* p = (uint*)prev;
    for (byte i = 0; i < 4; i++) {
        if (*m++ != *p++) return true;  // Changed.
    }
    return false; // No change.
}

byte m1[8];
byte m2[8];
byte kbd_on_irq_counter;

void kbd_on_irq() {
    bool changed;
    if (kbd_on_irq_counter & 1) {
        changed = kbd_scanner_changed(Keyboard.matrix1, Keyboard.matrix2);
        if (changed) SendKeyboardPacket(Keyboard.matrix1);
    } else {
        changed = kbd_scanner_changed(Keyboard.matrix2, Keyboard.matrix1);
        if (changed) SendKeyboardPacket(Keyboard.matrix2);
    }
    ++kbd_on_irq_counter;
}

int main() {
    kbd_on_irq();
}
