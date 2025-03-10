#ifndef _NEKOT1_KEYBOARD_H_
#define _NEKOT1_KEYBOARD_H_

struct keyboard {
    byte matrix[2][8];
    byte current_matrix;
} Keyboard MORE_DATA;

void KeyboardHandler(void);

#endif // _NEKOT1_KEYBOARD_H_
