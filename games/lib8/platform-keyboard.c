#include "nekot1/public.h"
#include "platform-keyboard.h"

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

int LookupKeyboard(gbyte sense, gbyte got) {
    gbyte s = BitNum(sense);
    gbyte g = BitNum(got);
    gbyte i = s + (g<<3);
    if (i < sizeof CocoKeyMap) {
        return CocoKeyMap[i];
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
            int z= LookupKeyboard(sense, got);
            // Printf("[ %d %d %d ]", sense, got, z);
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

int getchar() {
    static int prevchar;
    int c;
    do {
        do {
            WaitForTick();
            c = ScanKeyboard();
        } while (c == prevchar);
        prevchar = c;
    } while (c==0);
    return c;
}
