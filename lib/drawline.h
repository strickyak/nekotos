#ifndef _NEKOTOS_LIB_DRAWLINE_H_
#define _NEKOTOS_LIB_DRAWLINE_H_

#include "nekotos/lib/drawspot.h"

void PMode1DrawHorz(gbyte* fb, SpotDrawer draw, gbyte x, gbyte y, gbyte color,
                    gbyte len) {
  gbyte limit = x + len;
  for (gbyte i = x; i < limit; i++) {
    draw(fb, i, y, color);
  }
}

void PMode1DrawVirt(gbyte* fb, SpotDrawer draw, gbyte x, gbyte y, gbyte color,
                    gbyte len) {
  gbyte limit = y + len;
  for (gbyte i = y; i < limit; i++) {
    draw(fb, x, i, color);
  }
}

#endif  //  _NEKOTOS_LIB_DRAWLINE_H_
