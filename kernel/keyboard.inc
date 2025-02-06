#ifndef _KEYBOARD_INC_
#define _KEYBOARD_INC_

#include "kernel.inc"

struct keyboard {
    byte matrix1[8];
    byte matrix2[8];
};

#ifndef KEYBOARD_ADDR
#define KEYBOARD_ADDR 0x0150
#endif

// #define Keyboard (*(struct keyboard*)KEYBOARD_ADDR)

#endif // _KEYBOARD_INC_
