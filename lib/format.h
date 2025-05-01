#ifndef _NEKOTOS_LIB_FORMAT_H_
#define _NEKOTOS_LIB_FORMAT_H_

#include "kernel/public.h"
// #include "platform-text.h"

#include <stdarg.h>

#define TEXT_STR_MAX 30

#define EMIT(CHAR) \
  do {             \
    *p++ = (CHAR); \
  } while (0)

char* PPutStr(char* p, const char* s) {
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

const char PHexAlphabet[] = "0123456789ABCDEF";

char* PPutHex(char* p, gword x) {
  if (x > 15u) {
    p = PPutHex(p, x >> 4u);
  }
  EMIT(PHexAlphabet[15u & x]);
  return p;
}

gbyte PDivMod10(gword x, gword* out_div) {  // returns mod
  gword div = 0;
  while (x >= 10000) x -= 10000, div += 1000;
  while (x >= 1000) x -= 1000, div += 100;
  while (x >= 100) x -= 100, div += 10;
  while (x >= 10) x -= 10, div++;
  *out_div = div;
  return (gbyte)x;
}
char* PPutDec(char* p, gword x) {
  gword div;
  if (x > 9u) {
    // eschew div // PPutDec(x / 10u);
    PDivMod10(x, &div);
    p = PPutDec(p, div);
  }
  // eschew mod // PutChar('0' + (gbyte)(x % 10u));
  EMIT('0' + PDivMod10(x, &div));
  return p;
}

char* PPutSigned(char* p, int x) {
  if (x < 0) {
    x = -x;
    EMIT('-');
  }
  return PPutDec(p, x);
}

char* Vprintf(char* p, const char* format, va_list ap) {
  int max = TEXT_STR_MAX;

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
        case 'd': {
          int x = va_arg(ap, int);
          p = PPutSigned(p, x);
        } break;
        case 'u': {
          gword x = va_arg(ap, gword);
          p = PPutDec(p, x);
        } break;
        case 'x': {
          gword x = va_arg(ap, gword);
          p = PPutHex(p, x);
        } break;
        case 's': {
          char* x = va_arg(ap, char*);
          p = PPutStr(p, x);
        } break;
        default:
          EMIT(*s);
      };  // end switch
    }  // end if
  }
  *p = '\0';
  return p;
}
char* Sprintf(char* p, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  return Vprintf(p, format, ap);
}

#endif  // _NEKOTOS_LIB_FORMAT_H_
