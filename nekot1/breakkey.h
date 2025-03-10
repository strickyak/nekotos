#ifndef _N1_BREAKKEY_H_
#define _N1_BREAKKEY_H_

struct breakkey {
    bool volatile break_key_was_down;
} Breakkey;

// Break_Handler is called on interrupt.
void Breakkey_Handler(void);

#endif // _N1_BREAKKEY_H_
