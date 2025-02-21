struct keyboard {
    byte matrix[2][8];
    byte current_matrix;
};  // Instance is named Keyboard.

void KeyboardHandler(void);
