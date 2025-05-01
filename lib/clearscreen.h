#ifndef _NEKOTOS_LIB_CLEARSCREEN_H_
#define _NEKOTOS_LIB_CLEARSCREEN_H_

void PMode1ClearScreen(gbyte* fb, gbyte color) {
  color &= 3;
  gwob c;
  c.b[0] = c.b[1] = color | (color << 2) | (color << 4) | (color << 6);
  for (gword i = 0; i < 3 * 1024; i += 2) {
    gPoke2(fb + i, c.w);
  }
}

void PMode1ClearBoxAlignX4(gbyte* fb, gbyte nw_x, gbyte nw_y, gbyte wid,
                           gbyte hei, gbyte color) {
  gbyte x0 = nw_x >> 2;
  gbyte xn = (wid >> 2);
  gbyte y0 = nw_y;
  gbyte y9 = y0 + hei;
  gbyte c = QuadColor[color];

  gbyte* p = fb + ((gword)y0 << 5) + x0;
  for (gbyte y = y0; y < y9; y++) {
    gbyte* q = p;
    for (gbyte x = 0; x < xn; x++) {
      *q++ = c;
    }
    p += 32;
  }
}

#endif  // _NEKOTOS_LIB_CLEARSCREEN_H_
