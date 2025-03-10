#include "nekot1/private.h"

#include <stdarg.h>

static void AdvanceCursor() {
    ++Console.cursor;
    while (Console.cursor >= PANE_LIMIT) {
        // Scroll Pane upward
        for (gword p = PANE_BEGIN; p < PANE_LIMIT-32; p+=2) {
            Poke2(p, Peek2(p+32));
        }
        // Clear bottom Pane line
        for (gword p = PANE_LIMIT-32; p < PANE_LIMIT; p+=2) {
            Poke2(p, 0x2020);
        }
        // Move cursor back into bottom Pane line.
        Console.cursor -= 32;
    }
    Poke1(Console.cursor, 0xFF);
}

void PutRawByte(gbyte x) {
    Poke1(Console.cursor, x);
    AdvanceCursor();
}
void PutChar(char c) {
    Poke1(Console.cursor, 0x20);

    gbyte x = (gbyte)c; // Unsigned!
    if (x == '\n') {
            while (Console.cursor < PANE_LIMIT-1) {
                PutChar(' ');
            }
            PutChar(' ');
    } else if (x < 32) {
        // Ingore other control chars.
    } else if (x < 96) {
            PutRawByte(63&x);
    } else if (x < 128) {
            PutRawByte(x-96);
    } else {
            PutRawByte(x);
    }
}

void PutStr(const char* s) {
    for (; *s; s++) {
        PutChar(*s);
    }
}

#if 0
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
#if 0
void PutSigned(int x) {
    if (x<0) {
        x = -x;
        PutChar('-');
    }
    PutDec(x);
}
#endif
#if 0
void Console_Printf(const char* format, ...) {
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
                    PutDec(x);
                }
                break;
#endif
            case 'u':
            case 'x':
                {
                    gword x = va_arg(ap, gword);
                    PutDec(x);
                }
                break;
#if 0
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
    // Poke2(0, AdvanceCursor);

    // Draw a greenish bar across the top of the Console.
    for (gword p = CONSOLE_BEGIN; p < PANE_BEGIN; p+=2) {
        Poke2(p, 0x8C8C);  // greenish (in RGB or Composite) top bar
    }
    // Fill the body of the screen with spaces.
    for (gword p = PANE_BEGIN; p < PANE_LIMIT; p+=2) {
        Poke2(p, 0x2020);
    }
    // Draw a blueish bar across the bottom of the Console.
    for (gword p = PANE_LIMIT; p < CONSOLE_LIMIT; p+=2) {
        Poke2(p, 0xA3A3);  // blueish (in RGB or Composite) bottom bar
    }
    Console.cursor = PANE_LIMIT - 32;
    Poke1(Console.cursor, 0xFF);
}
