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

int LookupKeyboard(gbyte sense, gbyte got, gbyte shift) {
    gbyte s = BitNum(sense);
    gbyte g = BitNum(got);
    gbyte i = s + (g<<3);
    if (i < sizeof CocoKeyMap) {
        const char* map = (shift ? CocoKeyMapShifted : CocoKeyMap);
        return map[i];
    } else {
        return 0;
    }
}

int ScanKeyboard() {
    const gword out_port = Pia0PortB;
    const gword in_port = Pia0PortA;

    gbyte sense = 0x01;
    while (sense) {
        gPoke1(out_port, ~(gbyte)sense);
        gbyte got = 0x7F & ~gPeek1(in_port);
        if (got) {
            // Check SHIFT.
            gPoke1(out_port, ~(gbyte)0x80);
            gbyte shift = 0x40 & ~gPeek1(in_port);

            int z= LookupKeyboard(sense, got, shift);
            return z;
        }
        sense <<= 1;
    }
    return 0;
}

void WaitForTick() {
    gbyte a = gReal.ticks;
    while (a == gReal.ticks) {}
}

int prevchar;
int getchar() {
    int c;
    do {
        do {
            WaitForTick();
            c = ScanKeyboard();
        } while (c == prevchar);
        prevchar = c;
    } while (c==0);
    PutChar(c); // FOR KEYBOARD ECHO
    return c;
}
