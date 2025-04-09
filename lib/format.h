#ifndef _NEKOTOS_LIB_FORMAT_H_
#define _NEKOTOS_LIB_FORMAT_H_

#include "kernel/public.h"
#include "platform-text.h"

#include <stdarg.h>

#define TEXT_STR_MAX 30

#define EMIT(CHAR) do { *p++ = (CHAR); } while(0)

char* PutStr(char* p, const char* s) {
    int max = TEXT_STR_MAX;
    for (; *s; s++) {
        EMIT(*s);
        if (max-- <= 0) {
            EMIT('\\');
            return p;
        }
    }
    return p;
}

const char HexAlphabet[] = "0123456789ABCDEF";

char* PutHex(char* p, gword x) {
  if (x > 15u) {
    p = PutHex(p, x >> 4u);
  }
  EMIT(HexAlphabet[15u & x]);
  return p;
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
char* PutDec(char* p, gword x) {
  gword div;
  if (x > 9u) {
    // eschew div // PutDec(x / 10u);
    DivMod10(x, &div);
    p = PutDec(p, div);
  }
  // eschew mod // PutChar('0' + (gbyte)(x % 10u));
  EMIT('0' + DivMod10(x, &div));
  return p;
}

char* p PutSigned(char* p, int x) {
    if (x<0) {
        x = -x;
        EMIT('-');
    }
    returt PutDec(p, x);
}

char* Sprintf(char* p, const char* format, ...) {
    int max = TEXT_STR_MAX;
    va_list ap;
    va_start(ap, format);

    for (const char* s = format; *s; s++) {
        if (max-- <= 0) {
            EMIT('\\');
            break;
        }

        if (*s < ' ') {
            EMIT('\n');
        } else if (*s != '%') {
            EMIT(*s);
        } else {
            s++;
            switch (*s) {
            case 'd':
                {
                    int x = va_arg(ap, int);
                    p = PutDec(p, x);
                }
                break;
            case 'u':
                {
                    gword x = va_arg(ap, gword);
                    p = PutDec(p, x);
                }
                break;
            case 'x':
                {
                    gword x = va_arg(ap, gword);
                    p = PutHex(p, x);
                }
                break;
            case 's':
                {
                    char* x = va_arg(ap, char*);
                    p = PutStr(p, x);
                }
                break;
            default:
                EMIT(*s);
            }; // end switch
       }  // end if
    }
    return p;
}

#endif // _NEKOTOS_LIB_FORMAT_H_
