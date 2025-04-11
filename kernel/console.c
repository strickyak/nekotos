#include <stdarg.h>

#include "kernel/private.h"

void PutRawByte(gbyte x) {
  memcpy_words(STIFF_BEGIN, STIFF_BEGIN + 1, STIFF_LEN / 2);
  gPoke1(STIFF_LIMIT - 2, x);
  gPoke1(STIFF_LIMIT - 1, 0xEF);
}
void PutChar(char c) {
  gbyte x = (gbyte)c;  // Unsigned!
  if (x < 32) {
    PutChar(' ');
  } else if (x < 64) {
    PutRawByte(64 + x);  // Upper case.
  } else if (x < 96) {
    PutRawByte(x);  // Upper case.
  } else if (x < 128) {
    PutRawByte(x - 32);  // Lower case.
  } else {
    PutRawByte(x);  // Semigraphics.
  }
}

void PutStr(const char* s) {
  for (; *s; s++) {
    PutChar(*s);
  }
  PutChar(' ');
}

#if 1
char HexAlphabet[] = "0123456789ABCDEF";

void PutHex(gword x) {
  if (x > 15u) {
    PutHex(x >> 4u);
  }
  PutChar(HexAlphabet[15u & x]);
}
#endif
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
#if 1
void PutSigned(int x) {
  if (x < 0) {
    x = -x;
    PutChar('-');
  }
  PutDec(x);
}
#endif
#if 0
void Printf(const char* format, ...) {
    gbyte cc_value = gIrqSaveAndDisable();

    va_list ap;
    va_start(ap, format);

    for (const char* s = format; *s; s++) {
        if (*s < ' ') {
            PutChar('\n');
        } else if (*s != '%') {
            PutChar(*s);
        } else {
            s++;
            switch (*s) {
            case 'd':
#if 0
                {
                    int x = va_arg(ap, int);
                    PutSigned(x);
                }
                break;
#endif
            case 'u':
                {
                    gword x = va_arg(ap, gword);
                    PutDec(x);
                    PutChar('.');
                }
                break;
#if 1
            case 'x':
                {
                    gword x = va_arg(ap, gword);
                    PutChar('$');
                    PutHex(x);
                }
                break;
#endif
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
    gIrqRestore(cc_value);
}
#endif

void Console_Init() {
  // Draw a greenish bar across the top of the Console.
  for (gword p = CONSOLE_BEGIN; p < PANE_BEGIN; p += 2) {
    gPoke2(p, 0x8C8C);  // greenish (in RGB or Composite) top bar
  }

  // Fill the stiff part.
  memset_words(CONSOLE_BEGIN + 32, 0xEFEF, 16 * STIFF_LINES);

  // Draw a blueish bar across the bottom of the Console.
  for (gword p = PANE_LIMIT; p < CONSOLE_LIMIT; p += 2) {
    gPoke2(p, 0xA3A3);  // blueish (in RGB or Composite) bottom bar
  }
  gPoke1(CONSOLE_LIMIT - 31, 0xFF);
}
