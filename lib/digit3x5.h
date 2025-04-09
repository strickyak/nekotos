#ifndef _NEKOTOS_LIB_DIGIT3X5_H_
#define _NEKOTOS_LIB_DIGIT3X5_H_

#include "nekotos/lib/drawspot.h"

char Digit3x5CharGen[] =
    " 0 "
    "0 0"
    "0 0"
    "0 0"
    " 0 "

    " 1 "
    " 1 "
    " 1 "
    " 1 "
    " 1 "

    "22 "
    "  2"
    " 2 "
    "2  "
    "222"

    "33 "
    "  3"
    "33 "
    "  3"
    "33 "

    "4 4"
    "4 4"
    "444"
    "  4"
    "  4"

    "555"
    "5  "
    "55 "
    "  5"
    "55 "

    " 66"
    "6  "
    "666"
    "6 6"
    "666"

    "777"
    "  7"
    "  7"
    "  7"
    "  7"

    "888"
    "8 8"
    "888"
    "8 8"
    "888"

    "999"
    "9 9"
    "999"
    "  9"
    "  9";

// >>> [ x*15 for x in range(10) ]
// [0, 15, 30, 45, 60, 75, 90, 105, 120, 135]
gbyte DigitTimes15[] = { 0, 15, 30, 45, 60, 75, 90, 105, 120, 135 };

void PMode1DrawDigit3x5(gbyte* fb, SpotDrawer draw, gbyte x, gbyte y, gbyte color, gbyte digit) {
  char* pattern = Digit3x5CharGen + DigitTimes15[digit];  // that is, 15*digit.
  for (gword i = 0; i < 5; i++) {
    for (gword j = 0; j < 3; j++) {
      char spot = *pattern++;
      if (spot != ' ') {
        draw(fb, x + j, i + y, color);
      }
    }
  }
}

// x,y is NW corner.  20 wide, 5 deep.
// Returns leftmost x position drawn (so a minus can be inserted in front).
gbyte PMode1DrawDecimal3x5Unsigned(gbyte* fb, SpotDrawer draw, gbyte x, gbyte y, gbyte color, gword val) {
  gbyte a = 0, b = 0, c = 0, d = 0;
  gbyte left_most = 255;
  while (val >= 10000) {
    a++;
    val -= 10000;
  }
  while (val >= 1000) {
    b++;
    val -= 1000;
  }
  while (val >= 100) {
    c++;
    val -= 100;
  }
  while (val >= 10) {
    d++;
    val -= 10;
  }
  gbool show = gFALSE;
  if (a) {
    PMode1DrawDigit3x5(fb, draw, x, y, color, a);
    show = gTRUE;
    left_most = (x+0 < left_most? x+0 : left_most);
  }
  if (b || show) {
    PMode1DrawDigit3x5(fb, draw, x + 4, y, color, b);
    show = gTRUE;
    left_most = (x+4 < left_most? x+4 : left_most);
  }
  if (c || show) {
    PMode1DrawDigit3x5(fb, draw, x + 8, y, color, c);
    show = gTRUE;
    left_most = (x+8 < left_most? x+8 : left_most);
  }
  if (d || show) {
    PMode1DrawDigit3x5(fb, draw, x + 12, y, color, d);
    show = gTRUE;
    left_most = (x+12 < left_most? x+12 : left_most);
  }
  PMode1DrawDigit3x5(fb, draw, x + 16, y, color, (gbyte)val);
  left_most = (x+16 < left_most? x+16 : left_most);
  return left_most;
}

// x,y is NW corner.  24 wide, 5 deep.
void PMode1DrawDecimal3x5(gbyte* fb, SpotDrawer draw, gbyte x, gbyte y, gbyte color, int val) {
  if (val < 0) {
     gbyte left = PMode1DrawDecimal3x5Unsigned(fb, draw, x+4, y, color, -val);
     draw(fb, left-2, y+1, color);
     draw(fb, left-3, y+1, color);
     draw(fb, left-4, y+1, color);
  } else {
     return PMode1DrawDecimal3x5Unsigned(fb, draw, x+4, y, color, val);
  }
}

// x,y is NW corner.  20 wide, 5 deep.
void PMode1ClearDecimal3x5Unsigned(gbyte* fb, SpotDrawer draw, gbyte x, gbyte y, gbyte color) {
    for (gbyte i = 0; i < 5; i++) {
        PMode1DrawHorz(fb, draw, x, y+i, color, 20);
    }
}
// x,y is NW corner.  24 wide, 5 deep.
void PMode1ClearDecimal3x5(gbyte* fb, SpotDrawer draw, gbyte x, gbyte y, gbyte color) {
    for (gbyte i = 0; i < 5; i++) {
        PMode1DrawHorz(fb, draw, x, y+i, color, 24);
    }
}

#endif // _NEKOTOS_LIB_DIGIT3X5_H_
