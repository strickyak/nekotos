#include <stdarg.h>

static void AdvanceCursor() {
    ++Console.cursor;
    while (Console.cursor >= PANE_LIMIT) {
        // Scroll Pane upward
        for (uint p = PANE_BEGIN; p < PANE_LIMIT-32; p+=2) {
            Poke2(p, Peek2(p+32));
        }
        // Clear bottom Pane line
        for (uint p = PANE_LIMIT-32; p < PANE_LIMIT; p+=2) {
            Poke2(p, 0x2020);
        }
        // Move cursor back into bottom Pane line.
        Console.cursor -= 32;
    }
    Poke1(Console.cursor, 0xFF);
}

void PutChar(char c) {
    Poke1(Console.cursor, 0x20);

    byte x = (byte)c; // Unsigned!
    if (x == '\n') {  // Any control char is newline.
            while (Console.cursor < PANE_LIMIT-1) {
                PutChar(' '); // AdvanceCursor();            
            }
            PutChar(' ');
    } else if (x < 32) {
        // Ingore other control chars.
    } else if (x < 96) {
            Poke1(Console.cursor, 63&x); // 6-bit ASCII
            AdvanceCursor();
    } else if (x < 128) {
            Poke1(Console.cursor, x-96); // Convert Lower to Upper.
            AdvanceCursor();
    } else {
            Poke1(Console.cursor, x); // Semigraphics
            AdvanceCursor();
    }
}

void PutStr(const char* s) {
    for (; *s; s++) {
        PutChar(*s);
    }
}

char HexAlphabet[] = "0123456789ABCDEF";

void PutHex(word x) {
  if (x > 15u) {
    PutHex(x >> 4u);
  }
  PutChar(HexAlphabet[15u & x]);
}
byte DivMod10(word x, word* out_div) {  // returns mod
  word div = 0;
  while (x >= 10000) x -= 10000, div += 1000;
  while (x >= 1000) x -= 1000, div += 100;
  while (x >= 100) x -= 100, div += 10;
  while (x >= 10) x -= 10, div++;
  *out_div = div;
  return (byte)x;
}
void PutDec(word x) {
  word div;
  if (x > 9u) {
    // eschew div // PutDec(x / 10u);
    DivMod10(x, &div);
    PutDec(div);
  }
  // eschew mod // PutChar('0' + (byte)(x % 10u));
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
                {
                    int x = va_arg(ap, int);
                    PutSigned(x);
                }
                break;
            case 'u':
                {
                    uint x = va_arg(ap, uint);
                    PutDec(x);
                }
                break;
            case 'x':
                {
                    uint x = va_arg(ap, uint);
                    PutChar('$');
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

void Console_Init() {
    Poke2(0, AdvanceCursor);
    Vdg_Init();

    for (word p = CONSOLE_BEGIN; p < PANE_BEGIN; p+=2) {
        Poke2(p, 0x9C9C);
    }
    for (word p = PANE_BEGIN; p < PANE_LIMIT; p+=2) {
        Poke2(p, 0x2020);
    }
    for (word p = PANE_LIMIT; p < CONSOLE_LIMIT; p+=2) {
        Poke2(p, 0xE3E3);
    }
    Console.cursor = PANE_LIMIT - 32;
    Poke1(Console.cursor, 0xFF);
}
