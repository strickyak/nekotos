#ifndef _KERNEL_BREAKKEY_H_
#define _KERNEL_BREAKKEY_H_

struct breakkey {
    gbool volatile break_key_was_down;
} Breakkey;

// Break_Handler is called on interrupt.
void Breakkey_Handler(void);

#endif // _KERNEL_BREAKKEY_H_
