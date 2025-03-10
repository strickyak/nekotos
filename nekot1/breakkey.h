#ifndef _NEKOT1_BREAKKEY_H_
#define _NEKOT1_BREAKKEY_H_

struct breakkey {
    gbool volatile break_key_was_down;
} Breakkey;

// Break_Handler is called on interrupt.
void Breakkey_Handler(void);

#endif // _NEKOT1_BREAKKEY_H_
