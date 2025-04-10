#ifndef _KERNEL_CONSOLE_H_
#define _KERNEL_CONSOLE_H_

extern struct console {
    gword cursor;
} Console;

void Console_Init(void);

#define STIFF_LINES 4

#define STIFF_BEGIN  (CONSOLE_BEGIN + 32)
#define STIFF_LIMIT  (STIFF_BEGIN + STIFF_LEN)
#define STIFF_LEN    (32 * STIFF_LINES)

#define CONSOLE_BEGIN (gword)Cons
#define CONSOLE_LEN   512
#define CONSOLE_LIMIT (CONSOLE_BEGIN + CONSOLE_LEN)

#define PANE_BEGIN  (CONSOLE_BEGIN + 32 + STIFF_LINES*32)
#define PANE_LIMIT  (CONSOLE_LIMIT - 32)
#define PANE_LEN (PANE_LIMIT - PANE_BEGIN)

void PutChar(char c);
void PutStr(const char* s);
void PutDec(gword x);
gbyte DivMod10(gword x, gword* out_div);  // returns mod
void Printf(const char* format, ...);

#endif // _KERNEL_CONSOLE_H_
