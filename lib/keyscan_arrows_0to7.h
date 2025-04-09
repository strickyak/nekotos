#ifndef _NEKOTOS_LIB_KEYSCAN_ARROWS_0TO7_H_
#define _NEKOTOS_LIB_KEYSCAN_ARROWS_0TO7_H_

// ScanArrowsAnd0To7() will scan the keyboard
// for only 16 specific keys
//    X Y Z  UP DOWN LEFT RIGHT SPACE
//    0 1 2 3 4 5 6 7
// and return a 16-bit number representing
// which of those keys were down.

// Those 16 keys were enough for the `spacewar`
// game, and should suffice for lots of
// video games.

// WaitForKeyPressArrowsAnd0To7() continually
// calls ScanArrowsAnd0To7() and does not return
// until some key is pressed.  It does not attempt
// to debounce the keyboard, or eliminate
// duplicates, so be careful.  It returns the
// same kind of number as ScanArrowsAnd0To7().

// Call ScanArrowsAnd0To7() and get the result.
// If result is 0, none of these keys are down.
// If the bits #define'd below are set, these keys are down.
// That is, you can test for a key like this:
//
// gword scan = ScanArrowsAnd0To7();
// if (scan) {
//     if (scan & ArrowsAnd0To7_UP) {
//         DoUp();
//     } else if (scan & ArrowsAnd0To7_DOWN) {
//         DoDown();
//     } else {
//         DoSomethingElse():
//     }
// }

#define ArrowsAnd0To7_0     (1u<<0)
#define ArrowsAnd0To7_1     (1u<<1)
#define ArrowsAnd0To7_2     (1u<<2)
#define ArrowsAnd0To7_3     (1u<<3)
#define ArrowsAnd0To7_4     (1u<<4)
#define ArrowsAnd0To7_5     (1u<<5)
#define ArrowsAnd0To7_6     (1u<<6)
#define ArrowsAnd0To7_7     (1u<<7)

#define ArrowsAnd0To7_X     (1u<<8)
#define ArrowsAnd0To7_Y     (1u<<9)
#define ArrowsAnd0To7_Z     (1u<<10)

#define ArrowsAnd0To7_UP        (1u<<11)
#define ArrowsAnd0To7_DOWN      (1u<<12)
#define ArrowsAnd0To7_LEFT      (1u<<13)
#define ArrowsAnd0To7_RIGHT     (1u<<14)
#define ArrowsAnd0To7_SPACE     (1u<<15)

#define Pia0PortA   0xFF00
#define Pia0PortB   0xFF02

gword ScanArrowsAnd0To7() {
    gwob z;
    z.w = 0;
    // Ignore keyboard when not focused.
    if (!gKern.focus_game) return 0;

    for (gbyte bit=1; bit; bit<<=1) {
        gDisableIrq();
        gPoke1(Pia0PortB, ~bit);
        gbyte sense = gPeek1(Pia0PortA);
        gEnableIrq();

        if ((sense & 0x08) == 0) z.b[0] |= bit;
        if ((sense & 0x10) == 0) z.b[1] |= bit;
    }
    return z.w;
}

gword WaitForKeyPressArrowsAnd0To7() {
   gword w;
   do { w = ScanArrowsAnd0To7(); }
   while (w == 0);
   return w;
}

#endif // _NEKOTOS_LIB_KEYSCAN_ARROWS_0TO7_H_
