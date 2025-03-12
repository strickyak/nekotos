#include "nekot1/public.h"
#include "platform-keyboard.h"

void PutChar(char ch);
void PutStr(const char *s);

void _abort();
void _exit(int a);
void _fatal(const char* s, gword arg) {
    PutStr("** FATAL(%d)  %s **\n"); // , arg, s);
    // Printf("** FATAL(%d)  %s **\n", arg, s);
    _exit(7);
}
void _abort() {
    PutStr("** ABORT. \n");
    // Printf("** ABORT. \n");
    _exit(13);
}
void _exit(int a) {
    PutStr("** EXIT %d.  (HIT BREAK) \n");
    // Printf("** EXIT %d.  (HIT BREAK) \n", a);
    while (gKern.always_true) {
        gPoke2(0x3FE0, a++);
    }
    while (1) {}
}
// Return 0-7 if a bit is set; else returns 255.
gbyte BitNum(gbyte b) {
    for (gbyte i = 0; i < 8; i++) {
        if (b&1) return i;
        b >>= 1;
    }
    return 255;
}

const char CocoKeyMap[] =
    "@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\000\000\010\000 "
    "0123456789:;,-./"
    "\n";

const char CocoKeyMapShifted[] =
    "@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\000\000\010\000 "
    "\000!\"#$%&'()*+<=>?"
    "\n";

// Convert the keycode to ASCII.
gbyte LookupKeyboard(gbyte code) {
    gbyte shift = code & 0x80;
    code &= 0x7F;
    const char* map = (shift ? CocoKeyMapShifted : CocoKeyMap);

    if (code < sizeof CocoKeyMap) {
        return map[code];
    } else {
        return 0;
    }
}

gbyte ComputeKeycode(gbyte sense, gbyte got) {
    gbyte s = BitNum(sense);
    gbyte g = BitNum(got);
    gbyte code = s + (g<<3);
    return code;
}

#define  SK_NO_KEY_DOWN  127

// Return keycode with the property that only the high bit
// is changed by shift.  Return 127 if no key is down.
int ScanKeyboard() {
    const gword out_port = Pia0PortB;
    const gword in_port = Pia0PortA;

    gbyte sense = 0x01;
    gbyte shift = 0;
    while (sense) {
        gPoke1(out_port, ~(gbyte)sense);
        gbyte got = 0x7F & ~gPeek1(in_port);
        if (got) {
            // Check SHIFT.
            gPoke1(out_port, ~(gbyte)0x80);
            shift = 0x40 & ~gPeek1(in_port);

            gbyte code = ComputeKeycode(sense, got);
            if (shift) code |= 0x80;  // add high bit
            return code;
        }
        sense <<= 1;
    }
    return SK_NO_KEY_DOWN;
}

void WaitForTick() {
    gbyte a = gReal.ticks;
    while (a == gReal.ticks) {}
}

int prev_code;
int getchar() {
    int code;
    do {
        do {
            WaitForTick();
            code = ScanKeyboard();
            // Compare the key held down without regard for the shift,
            // so you can let go of shift before the key,
            // without creating phantom unshifted keystrokes.
        } while ((code & 0x7F) == (prev_code & 0x7F));
        prev_code = code;
    } while (code==SK_NO_KEY_DOWN);

    gbyte c = LookupKeyboard(code);
    if (c) PutChar(c); // FOR KEYBOARD ECHO
    return c;
}
