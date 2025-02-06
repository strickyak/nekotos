#include "kernel.inc"

void seconds_handler() {
    if (Kernel.seconds < 59) {
        ++Kernel.seconds;
    } else {
        Kernel.seconds = 0;
        Kernel.seconds_func();
    }
}

void ticks_handler() {
    if (Kernel.ticks < 9) {
        ++Kernel.ticks;
    } else {
        Kernel.ticks = 0;
        Kernel.seconds_func();
    }
}

void irq_handler() {
    if (Kernel.sub_ticks < 5) {
        ++Kernel.sub_ticks;
    } else {
        Kernel.sub_ticks = 0;
        Kernel.tick_func();
    }
}

byte kbd_value;
byte kbd_modified;

bool kbd_found(byte probe, byte seen, byte value) {
    if (seen == 0x40 && probe > 0x40) {
        return 0;  // No real keystroke -- just modifiers.
    }

    if (value == kbd_value) {
    } else {
        kbd_value = value;
        kbd_modified = 000000000;
    }
}

byte bit_number(byte a) {
    byte b = 1;
    for (byte i=1; i<=8; i++) {
        if (a&b) return i;  // bit found
        b <<= 1;
    }
    return 0;  // if no bit found
}

byte kbd_matrix[8];

byte kbd_maybe(byte x) {
    return x; // TODO
}

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
        changed = kbd_scanner_changed(m1, m2);
        if (changed) SendKeyboardPacket(m1);
    } else {
        changed = kbd_scanner_changed(m2, m1);
        if (changed) SendKeyboardPacket(m2);
    }
    ++kbd_on_irq_counter;
}


bool kbd_scanner() {
    volatile byte* const out_port = (byte*)0xFF02;
    volatile byte* const in_port = (byte*)0xFF00;
    byte value = 1;
    byte modifiers = 0;

    *out_port = 0;
    byte anything = 0x7F & ~(*in_port);
    if (!anything) {
        goto NOT_FOUND;
    }

  {
    byte i = 8;
    byte probe = 1;
    byte* p = kbd_matrix;
    do {
        *out_port = ~(byte)(probe);
        *p++ = 0x7F & ~(*in_port);
    } while (i--);
  }

  {
    byte* p = kbd_matrix;
    for (byte k = 0; k < 3; k++) {
        byte x = *p++;
        if (x) {
            return kbd_maybe((k<<3)+bit_number(x));
        }
    }
  }

NOT_FOUND:
  kbd_value = 0;
  kbd_modified = 0;
  return 0;

#if 0
        const byte seen = 0x7F & ~(*in_port);
        if (seen) {
            if (seen & 0x40) {
                modifiers |= probe;
            }
            const byte bit_limit = (probe > 0x40) ? 0x40 : 0x80;
            for (byte bit = 1; bit < bit_limit; bit <<=1) {
                if (seen & bit) {
                    return kbd_found(probe, seen, value);
                }
            }
        } else {
            value += 8;
        }
    }
#endif
}

void kernel_init() {
}
void kernel_main() {
}

int main() {
    bool x = kbd_scanner();
    *(volatile bool*)0x7000 = x;
}
