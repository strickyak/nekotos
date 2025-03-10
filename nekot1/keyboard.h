#ifndef _g_KEYBOARD_H_
#define _g_KEYBOARD_H_

struct keyboard {
    byte matrix[2][8];
    byte current_matrix;
} Keyboard MORE_DATA;

void KeyboardHandler(void);

#endif // _g_KEYBOARD_H_
