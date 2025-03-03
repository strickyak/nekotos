#ifndef _N1_CONSOLE_H_
#define _N1_CONSOLE_H_

struct console {
    word cursor;
} Console;

void Console_Init(void);

#define CONSOLE_BEGIN (word)Cons
#define CONSOLE_LEN   0x200
#define CONSOLE_LIMIT (CONSOLE_BEGIN + CONSOLE_LEN)

#define PANE_BEGIN  (CONSOLE_BEGIN + 32)
#define PANE_LIMIT  (CONSOLE_LIMIT - 32)
#define PANE_LEN (PANE_LIMIT - PANE_BEGIN)

void PutChar(char c);
void PutStr(const char* s);
void Console_Printf(const char* format, ...);

#endif // _N1_CONSOLE_H_
