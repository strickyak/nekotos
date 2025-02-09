struct console {
    word cursor;
};
void Console_Init(void);

#define CONSOLE_BEGIN 0x200
#define CONSOLE_LIMIT 0x400
#define CONSOLE_LEN (CONSOLE_LIMIT - CONSOLE_BEGIN)

#define PANE_BEGIN  (CONSOLE_BEGIN + 32)
#define PANE_LIMIT  (CONSOLE_LIMIT - 32)
#define PANE_LEN (PANE_LIMIT - PANE_BEGIN)

void PutChar(char c);
void PutStr(const char* s);
void Printf(const char* format, ...);
