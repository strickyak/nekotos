#ifndef _KERNEL_KEYBOARD_H_
#define _KERNEL_KEYBOARD_H_

struct keyboard {
    gbyte matrix[2][8];
    gbyte current_matrix;
} Keyboard gZEROED;

void KeyboardHandler(void);

#endif // _KERNEL_KEYBOARD_H_
