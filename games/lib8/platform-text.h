#ifndef _PLATFORM_TEXT_H_
#define _PLATFORM_TEXT_H_

#define TEXT_BEGIN ((gword)TextScreen)
#define TEXT_LEN   0x200
#define TEXT_LIMIT (TEXT_BEGIN + TEXT_LEN)

void PutChar(char c);
void PutStr(const char* s);
void PutDec(gword x);
gbyte DivMod10(gword x, gword* out_div);  // returns mod
void Printf(const char* format, ...);

void Text_Init(void);

#endif // _PLATFORM_TEXT_H_
