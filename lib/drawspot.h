#ifndef _NEKOTOS_LIB_DRAWSPOT_
#define _NEKOTOS_LIB_DRAWSPOT_

typedef void (*SpotDrawer)(gbyte* fb, gbyte x, gbyte y, gbyte color);

void PMode1DrawSpot(gbyte* fb, gbyte x, gbyte y, gbyte color) {
  if (x >= 128) return;  // gFatal("X GT", x);
  if (y >= 96) return;   // gFatal("Y GT", y);

  gbyte xshift = x & 3;           // mod 4
  gbyte xdist = x >> 2;           // div 4
  gword ydist = ((gword)y) << 5;  // times 32
  gword addr = (gword)fb + (gword)xdist + ydist;

  gbyte b = gPeek1(addr);
  gbyte bitshift = (3 - xshift) << 1;
  gbyte mask = ~(3 << bitshift);
  b = (b & mask) | (color << bitshift);

  gPoke1(addr, b);
}

void PMode1DrawSpotXor(gbyte* fb, gbyte x, gbyte y, gbyte color) {
  gbyte xshift = x & 3;           // mod 4
  gbyte xdist = x >> 2;           // div 4
  gword ydist = ((gword)y) << 5;  // times 32
  gword addr = (gword)fb + xdist + ydist;
  gPXOR(addr, (color << ((3 - xshift) << 1)));
}

void PMode1DrawSpotModulo(gbyte* fb, int x, int y, gbyte color) {
  while (x < 0) x += 128;
  while (y < 0) x += 96;
  while (x > 128) x -= 128;
  while (y > 96) x -= 96;
  PMode1DrawSpot(fb, (gbyte)x, (gbyte)y, color);
}

void PMode1DrawSpotXorModulo(gbyte* fb, int x, int y, gbyte color) {
  while (x < 0) x += 128;
  while (y < 0) x += 96;
  while (x > 128) x -= 128;
  while (y > 96) x -= 96;
  PMode1DrawSpotXor(fb, (gbyte)x, (gbyte)y, color);
}

#endif  //  _NEKOTOS_LIB_DRAWSPOT_
