#include "nekot1/public.h"
#include "platform-text.h"

#include <stdarg.h>

#define TEXT_STR_MAX 30

gword cursor;

void AdvanceCursor() {
    ++cursor;
    while (cursor >= TEXT_LIMIT) {
        // Scroll Pane upward
        for (gword p = TEXT_BEGIN; p < TEXT_LIMIT-32; p+=2) {
            gPoke2(p, gPeek2(p+32));
        }
        // Clear bottom Pane line
        for (gword p = TEXT_LIMIT-32; p < TEXT_LIMIT; p+=2) {
            gPoke2(p, 0x2020);
        }
        // Move cursor back into bottom Pane line.
        cursor -= 32;
    }
    gPoke1(cursor, 0xFF);
}

void PutRawByte(gbyte x) {
    gPoke1(cursor, x);
    AdvanceCursor();
    for (gword i = 0; i < 0x100; i++) {
        gPoke2(0x3FFE, i);
    }
}
void PutChar(char c) {
    gPoke1(cursor, 0x20);

    gbyte x = (gbyte)c; // Unsigned!
    if (x == '\n') {
            while (cursor < TEXT_LIMIT-1) {
                PutChar(' ');
            }
            PutChar(' ');
    } else if (x < 32) {
            PutChar('_'); // back arrow for Control Chars
    } else if (x < 96) {
            PutRawByte(63&x);
    } else if (x < 128) {
            PutRawByte(x-96);
    } else {
            PutRawByte(x);
    }
}

void PutStr(const char* s) {
    int max = TEXT_STR_MAX;
    for (; *s; s++) {
        PutChar(*s);
        if (max-- <= 0) {
            PutChar('\\');
            return;
        }
    }
}

char HexAlphabet[] = "0123456789ABCDEF";

void PutHex(gword x) {
  if (x > 15u) {
    PutHex(x >> 4u);
  }
  PutChar(HexAlphabet[15u & x]);
}

gbyte DivMod10(gword x, gword* out_div) {  // returns mod
  gword div = 0;
  while (x >= 10000) x -= 10000, div += 1000;
  while (x >= 1000) x -= 1000, div += 100;
  while (x >= 100) x -= 100, div += 10;
  while (x >= 10) x -= 10, div++;
  *out_div = div;
  return (gbyte)x;
}
void PutDec(gword x) {
  gword div;
  if (x > 9u) {
    // eschew div // PutDec(x / 10u);
    DivMod10(x, &div);
    PutDec(div);
  }
  // eschew mod // PutChar('0' + (gbyte)(x % 10u));
  PutChar('0' + DivMod10(x, &div));
}

void PutSigned(int x) {
    if (x<0) {
        x = -x;
        PutChar('-');
    }
    PutDec(x);
}

void Printf(const char* format, ...) {
    int max = TEXT_STR_MAX;
    va_list ap;
    va_start(ap, format);

    for (const char* s = format; *s; s++) {
        if (max-- <= 0) {
            PutChar('\\');
            break;
        }

        if (*s < ' ') {
            PutChar('\n');
        } else if (*s != '%') {
            PutChar(*s);
        } else {
            s++;
            switch (*s) {
            case 'd':
                {
                    int x = va_arg(ap, int);
                    PutDec(x);
                }
                break;
            case 'u':
                {
                    gword x = va_arg(ap, gword);
                    PutDec(x);
                }
                break;
            case 'x':
                {
                    gword x = va_arg(ap, gword);
                    PutHex(x);
                }
                break;
            case 's':
                {
                    char* x = va_arg(ap, char*);
                    PutStr(x);
                }
                break;
            default:
                PutChar(*s);
            }; // end switch
       }  // end if
    }
}

void Text_Init() {
    // Fill the body of the screen with spaces.
    for (gword p = TEXT_BEGIN; p < TEXT_BEGIN+512; p+=2) {
        gPoke2(p, 0x2F2F); // slashes.
    }
    cursor = TEXT_LIMIT - 32;
    gPoke1(cursor, 0xFF);
}
