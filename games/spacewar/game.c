#include "kernel/public.h"

gSCREEN(Screen, 12);  // Screen for PMODE1

// ColorSet 0 for PMode 1
#define Green0  0
#define Yellow0 1
#define Blue0   2
#define Red0    3

// ColorSet 1 for PMode 1
#define Buff1   0
#define Cyan1   1
#define Magenta1 2
#define Orange1 3

#define Pia0PortA 0xFF00
#define Pia0PortB 0xFF02

#define NUM_SHIPS 4

void Debug(const char* fmt, ...) { }

struct body {
  gword x, y;
  int r, s;
  int score;
  gbyte direction;
  gbyte ttl;
  gword dings[NUM_SHIPS];
};

gbyte mode;
struct body TheShips[NUM_SHIPS] gZEROED;
struct body TheMissiles[NUM_SHIPS] gZEROED;

// Temporary
int displayed_score[NUM_SHIPS] gZEROED;

////////////////////////////////////////////////////////

void Delay(gword n) {
  // if (IsThisGomar()) return;  // delay not needed if Gomar.

  while (n--) {
#ifdef __GNUC__
    asm volatile("mul" : : : "d", "b", "a");
    asm volatile("mul" : : : "d", "b", "a");
    asm volatile("mul" : : : "d", "b", "a");
    asm volatile("mul" : : : "d", "b", "a");
    asm volatile("mul" : : : "d", "b", "a");
#else
    asm {
      mul
      mul
      mul
      mul
      mul
    }
#endif
  }
}

void Pia1bOn(gbyte x) { *(volatile gbyte*)0xFF22 |= x; }
void Pia1bOff(gbyte x) { *(volatile gbyte*)0xFF22 &= ~x; }

void Beep(gbyte n, gbyte f) {
  for (gbyte i = 0; i < n; i++) {
    Pia1bOn(0x02);
    Delay(f);
    Pia1bOff(0x02);
    Delay(f);
  }
}

// gcc6809 -f'whole-program' doesn't like libgcc runtime library calls.
// So here are ShiftRight and ArithShiftRight tht avoid library calls.
gword ShiftLeft(gword x, gbyte count) {
  for (gbyte i = 0; i < count; i++) {
    x <<= 1;
  }
  return x;
}

gbyte ByteShiftLeft(gbyte x, gbyte count) {
  for (gbyte i = 0; i < count; i++) {
    x <<= 1;
  }
  return x;
}

int ShiftRight(int x, gbyte count) {
  gword z = (gword)x;
  for (gbyte i = 0; i < count; i++) {
    z >>= 1;
  }
  return (int)z;
}

int ArithShiftRight(int x, gbyte count) {
  gword z = (gword)x;
  for (gbyte i = 0; i < count; i++) {
    z >>= 1;
    if (x & 0x8000) z |= 0x8000;
  }
  return (int)z;
}

////////////////////////////////////////////////////////

char Digits[] =
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

void AssertEQ(gword a, gword b) {
  if (a != b) {
    gFatal(":EQ", b);
  }
}

void AssertLE(gword a, gword b) {
  if (a > b) {
    gFatal(":LE", b);
  }
}

////////////////////////////////////////////////////////

// We only scan this keyboard row 3:
#define KEY_X (1 << 0)
#define KEY_Y (1 << 1)
#define KEY_Z (1 << 2)
#define KEY_UP (1 << 3)
#define KEY_DOWN (1 << 4)
#define KEY_LEFT (1 << 5)
#define KEY_RIGHT (1 << 6)
#define KEY_SPACE (1 << 7)

// Returns a bitmap of the above bits, 1 if key down, otherwise 0.
gbyte RelevantKeysDown() {
  const gbyte row3 = (1 << 3);
  gbyte z = 0;
  for (gbyte b = 0x80; b; b >>= 1) {
    gPoke1(0xFF02, 0xFF ^ b);  // Key sense is active low.
    gbyte c = gPeek1(0xFF00);
    if ((c & row3) == 0) z |= b;
  }
  gPoke1(0xFF02, 0xFF);  // turn off the current.
  return z;
}

struct spacewar_msg {
  gbyte magic_aa;
  gbyte ship_num;
  struct body ship, missile;
  gword dings[NUM_SHIPS];
};

struct gamecast send_me;

void BroadcastShip(int ship_num) {
  if (mode == 'S') return;

  //struct body* ptr_ship = TheShips + ship_num;
  //struct body* ptr_missile = TheMissiles + ship_num;

  send_me.sender = 0;
  send_me.flags = 0;
  struct spacewar_msg* msg = (struct spacewar_msg*) send_me.payload;
  msg->magic_aa = 0xAA;
  msg->ship_num = ship_num;
  msg->ship = TheShips[ship_num];
  msg->missile = TheMissiles[ship_num];

  gSendCast(&send_me, 2 + sizeof *msg);
}

gbyte Ships[] = {
#include "spacewar-ships.h"
};

gbyte Gravity[] = {
#include "spacewar-gravity.h"
};

#define GRAF_LEN 0xC00  // (i.e. 3072 bytes) for G3CMode ("PMode1")

#define W 128u
#define H 96u

void ComputeGravity(gword x, gword y, int* gx_out, int* gy_out) {
  *gx_out = *gy_out = 0;
  x >>= 8;  // No fractional part.
  y >>= 8;  // No fractional part.

  // tx: table x, reduced to one quadrant, 0..W/2-1.
  gword tx = (x > W / 2) ? W - 1 - x : x;
  // ty: table y, reduced to one quadrant, 0..H/2-1.
  gword ty = (y > H / 2) ? H - 1 - y : y;
  Debug("G: x,y=%d,%d tx,ty=%d,%d\n", x, y, tx, ty);

  int abs_gx, abs_gy;
  if (tx >= W / 2 - W / 8 && ty >= H / 2 - H / 8) {
    // High resolution
    gword ix = (tx - (W / 2 - W / 8));  // index x
    gword iy = (ty - (H / 2 - H / 8));  // index y
    gword index = /* 2*(H/2)*(W/2) + */ (ix << 2) +
                 (iy << (2 + 3));  // four-gbyte records.
    abs_gx = (Gravity[index] << 8) + Gravity[index + 1];
    abs_gy = (Gravity[index + 2] << 8) + Gravity[index + 3];
    Debug("G:HI:    ix,iy=%d,%d index=%d  abs=%d,%d\n", ix, iy, index, abs_gx,
          abs_gy);
  } else {
    // Low resolution
#define ANTTI_GRAVITY 0  // 4
#if 1
    gbyte tiny_grav = 3;
    if (tx + ty < H / 4)
      tiny_grav = 1;
    else if (tx + ty < H / 2)
      tiny_grav = 2;

    abs_gx = abs_gy = (tiny_grav << ANTTI_GRAVITY);
#else
    gword ix = (tx >> 1);                       // index x
    gword iy = (ty >> 1);                       // index y
    gword index = (ix << 1) + (iy << (1 + 5));  // two-gbyte records.
    abs_gx = Gravity[index];
    abs_gy = Gravity[index + 1];
#endif
    Debug("G:LO:    abs=%d,%d\n", abs_gx, abs_gy);
  }

#define GL 1000
  abs_gx = (abs_gx > GL) ? GL : abs_gx;
  abs_gy = (abs_gy > GL) ? GL : abs_gy;

  abs_gx = ArithShiftRight(abs_gx, ANTTI_GRAVITY);
  abs_gy = ArithShiftRight(abs_gy, ANTTI_GRAVITY);
  *gx_out = (x < W / 2) ? abs_gx : -abs_gx;
  *gy_out = (y < H / 2) ? abs_gy : -abs_gy;
  Debug("G:RET    out = %d,%d\n", *gx_out, *gy_out);
}



/////////////////////////////////////////////////////////////

typedef void (*SpotDrawer)(gbyte* fb, gbyte x, gbyte y, gbyte color);

void DrawSpot(gbyte* fb, gbyte x, gbyte y, gbyte color) {
  gbyte xshift = x & 3;  // mod 4
  gbyte xdist = x >> 2;  // div 4
  gword addr = (gword)fb + xdist + ((gword)y << 5);
  gbyte b = gPeek1(addr);
  gbyte bitshift = (3 - xshift) << 1;
  gbyte mask = ~(3 << bitshift);
  b = (b & mask) | (color << bitshift);
  gPoke1(addr, b);
}
void DrawSpotXor(gbyte* fb, gbyte x, gbyte y, gbyte color) {
  gbyte xshift = x & 3;  // mod 4
  gbyte xdist = x >> 2;  // div 4
  gword addr = (gword)fb + xdist + ((gword)y << 5);
  gPXOR(addr, (color << ((3 - xshift) << 1)));
}
void DrawHorz(gbyte* fb, gbyte x, gbyte y, gbyte color, gbyte len, SpotDrawer spot) {
    gbyte last = x + len;
    for (gbyte i = x; i <= last; i++) {
        spot(fb, i, y, color);
    }
}
void DrawVirt(gbyte* fb, gbyte x, gbyte y, gbyte color, gbyte len, SpotDrawer spot) {
    gbyte last = y + len;
    for (gbyte i = y; i <= last; i++) {
        spot(fb, x, i, color);
    }
}


void DrawDigit(gbyte* fb, gbyte x, gbyte y, gbyte color, gbyte digit) {
  char* pattern = Digits + (digit << 4) - digit;  // that is, 15*digit.
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 3; j++) {
      char spot = *pattern++;
      if (spot != ' ') {
        DrawSpot(fb, x + j, i + y, (color < 4) ? color : (i<2) ? 3 : (i<3) ? 2 : 1);
      }
    }
  }
}

// Returns leftmost x position drawn (so a minus can be inserted in front).
gbyte DrawDecimal(gbyte* fb, gbyte x, gbyte y, gbyte color, int val) {
  gbyte a = 0, b = 0, c = 0, d = 0;
  gbyte left_most = 255;
  if (val < 0) {
     gbyte left = DrawDecimal(fb, x, y, color, -val);
     DrawSpot(fb, left-2, y+1, 2);
     DrawSpot(fb, left-3, y+1, 2);
     DrawSpot(fb, left-4, y+1, 2);
     return left-4;
  }
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
    DrawDigit(fb, x, y, color, a);
    show = gTRUE;
    left_most = (x+0 < left_most? x+0 : left_most);
  }
  if (b || show) {
    DrawDigit(fb, x + 4, y, color, b);
    show = gTRUE;
    left_most = (x+4 < left_most? x+4 : left_most);
  }
  if (c || show) {
    DrawDigit(fb, x + 8, y, color, c);
    show = gTRUE;
    left_most = (x+8 < left_most? x+8 : left_most);
  }
  if (d || show) {
    DrawDigit(fb, x + 12, y, color, d);
    show = gTRUE;
    left_most = (x+12 < left_most? x+12 : left_most);
  }
  DrawDigit(fb, x + 16, y, color, (gbyte)val);
  left_most = (x+16 < left_most? x+16 : left_most);
  return left_most;
}

void DrawSpot2(gbyte* fb, int x, int y, gbyte color) {
	while (x<0) x += 128;
	while (y<0) x += 96;
	while (x>128) x -= 128;
	while (y>96) x -= 96;
	DrawSpot(fb, x, y, color);
}

void DrawShip(gbyte* fb, struct body* p, gword ship, gbool isaMissile) {
  if (!p->ttl) return;

  gbyte mask = 0xFF;  // four of color "11": 11111111
  switch (ship) {
    case 0:
      mask = 0x55;  // four of color "01": 01010101
      break;
    case 1:
      mask = 0xAA;  // four of color "10": 10101010
      break;
  }

  int x = (p->x >> 8) - 2;
  int y = (p->y >> 8) - 2;
  gword xshift = x & 3;  // mod 4 // pixel offset within gbyte
  gword xdist = x >> 2;  // div 4
  gword index = (xshift * 5 * 2) + (p->direction * 5 * 4 * 2);
  gword yval = y;

  if (isaMissile) {
    DrawSpot2(fb, x+2, y+2, 2);
    // gword v = (gword)Screen + xdist + (yval << 5);
    // gword c0 = 0xC0;
    // gbyte spot = ShiftRight(c0 & mask, xshift);
    // gPXOR(v + 2, spot);
  } else {
    for (gbyte row = 0; row < 5 * 32; row += 32) {
      if (ship == 3) {
      	if (row == 64)  mask=0xAA;  // change color
        else if (row == 96)  mask=0x55;  // change color
      }
      gword v = (gword)Screen + xdist + (yval << 5) + row;
      if (v < (gword)Screen + 3 * 1024) v += 3 * 1024;  // wrap
      if (v >= (gword)Screen + 3 * 1024) v -= 3 * 1024;  // wrap
      gPXOR(v + 0, mask & Ships[index++]);
      gPXOR(v + 1, mask & Ships[index++]);
    }
  }

  // debugging: box it
#if 0
  x = (p->x >> 8);
  y = (p->y >> 8);
  for (gbyte i = 0; i < 9; i++) {
  	DrawSpot2(fb, x-4, y-4+i, 1);
  	DrawSpot2(fb, x+4, y-4+i, 1);
  	DrawSpot2(fb, x-4+i, y-4, 1);
  	DrawSpot2(fb, x-4+i, y+4, 1);
  }
#endif
}
#define XWRAP (gword)(W * 256u)
#define YWRAP (gword)(H * 256u)

gword GraphicsAddr(gword x, gword y) {
    return (gword)Screen + (x >> 2) + (y << 5);
}

void DrawSun(gbyte* fb, gword g) {
	DrawSpot(fb, W/2, H/2, 3&(g>>2));
/*
  gbyte* p = (gbyte*)Screen + 1024 + 512;
  p[17] += 64;
*/
}

#define ACCEL 8  // 3
int AccelR[16] = {
    5 * ACCEL,  4 * ACCEL,  3 * ACCEL,  1 * ACCEL,  0 * ACCEL,  -1 * ACCEL,
    -3 * ACCEL, -4 * ACCEL, -5 * ACCEL, -4 * ACCEL, -3 * ACCEL, -1 * ACCEL,
    0 * ACCEL,  1 * ACCEL,  3 * ACCEL,  4 * ACCEL,
};
int AccelS[16] = {
    0 * ACCEL,  1 * ACCEL,  3 * ACCEL,  4 * ACCEL,  5 * ACCEL,  4 * ACCEL,
    3 * ACCEL,  1 * ACCEL,  0 * ACCEL,  -1 * ACCEL, -3 * ACCEL, -4 * ACCEL,
    -5 * ACCEL, -4 * ACCEL, -3 * ACCEL, -1 * ACCEL,
};


gwob ScanArrowsAnd0To7() {
    gwob z;
    z.w = 0;
    if (!gKern.in_game) return z;

    gDisableIrq();
    for (gbyte bit=1; bit; bit<<=1) {
        gPoke1(Pia0PortB, ~bit);
        gbyte sense = gPeek1(Pia0PortA);
        if ((sense & 0x08) == 0) z.b[0] |= bit;
        if ((sense & 0x10) == 0) z.b[1] |= bit;
    }
    gEnableIrq();
    return z;
}

void ClearScreen(gbyte* fb, gbyte color) {
    color &= 3;
    gwob c;
    c.b[0] = c.b[1] = color | (color<<2) | (color<<4) | (color<<6);
    for (gword i = (gword)fb; i < (gword)fb + 3*1024; i+=2) {
        gPoke2(i, c.w);
    }
}

void WaitFor60HzTick() {
    gbyte t = gPeek1(&gReal.ticks);
    while (gPeek1(&gReal.ticks) == t) {}
}
void WaitForKeyPressArrowsAnd0To7() {
   gwob w;
   do { w = ScanArrowsAnd0To7(); }
   while (w.w == 0);
}

void FireMissile(gbyte who) {
  struct body* s = TheShips + who;
  struct body* m = TheMissiles + who;
  *m = *s;  // copy TheShips's position and momentum
#define M_SPEED 30
  m->x += M_SPEED * AccelR[s->direction];
  m->y -= M_SPEED * AccelS[s->direction];
  m->r += M_SPEED * AccelR[s->direction];
  m->s -= M_SPEED * AccelS[s->direction];
  m->ttl = 148;
}

gbool DetectHits(struct body* my_missile, gbyte my_num) {
  gbool z = gFALSE;
  for (gbyte i = 0; i < NUM_SHIPS; i++) {
    if (i == my_num) continue;  // dont count self-hits
    struct body* p = TheShips + i;
    if (!p->ttl) continue;  // that ship does not exist
    gword dx = (my_missile->x > p->x) ? (my_missile->x - p->x)
                                     : (p->x - my_missile->x);
    gword dy = (my_missile->y > p->y) ? (my_missile->y - p->y)
                                     : (p->y - my_missile->y);
    gword dist = dx + dy;
#define NEARBY 0x0400
    if (dist < NEARBY) {
      TheShips[my_num].score+=3;  // Give me a point.
      TheShips[my_num].dings[i]+=2;  // Ding the victim.
      my_missile->ttl = 0;  // expire the missile.
      z = gTRUE;
      // continue to hit other ships simultaneously!
    }
  }
  return z;
}

void AdvanceBody(struct body* p, int ship, gbool useGravity) {
#define SLOW 3
  int new_x = (int)(p->x) + ArithShiftRight(p->r, SLOW);
  int new_y = (int)(p->y) + ArithShiftRight(p->s, SLOW);
  // Wrap to stay on torus.
  while (new_x < 0) {
    new_x += XWRAP;
  }
  while (new_y < 0) {
    new_y += YWRAP;
  }
  while (new_x >= XWRAP) {
    new_x -= XWRAP;
  }
  while (new_y >= YWRAP) {
    new_y -= YWRAP;
  }
  Debug("xy=%d,%d  new=%d,%d  r,s=%d,%d\n", p->x, p->y, new_x, new_y, p->r,
        p->s);

  if (useGravity) {
    int gx, gy;
    ComputeGravity(new_x, new_y, &gx, &gy);
    p->r += gx;
    p->s += gy;
    Debug("grav=%d,%d  r,s=%d,%d\n", gx, gy, p->r, p->s);
  }

#define SpeedLimit 6000

  p->r = (p->r < -SpeedLimit)  ? -SpeedLimit
         : (p->r > SpeedLimit) ? SpeedLimit
                               : p->r;
  p->s = (p->s < -SpeedLimit)  ? -SpeedLimit
         : (p->s > SpeedLimit) ? SpeedLimit
                               : p->s;
  p->x = (gword)new_x;
  p->y = (gword)new_y;
}

void DrawScores() {
  for (gbyte i = 0; i < NUM_SHIPS; i++) {
    struct body* p = TheShips + i;
    int score = p->score;  // My claimed score.

    for (gbyte j = 0; j < NUM_SHIPS; j++) {  // Deduct dings.
      struct body* q = TheShips + j;
      score -= q->dings[i];
    }
    // if (score<0) score=0;  // Be kind.

    if (score != displayed_score[i]) {
      DrawDecimal(Screen, /*x=*/100, /*y=*/i << 3, /*color=*/i + 1,
                  displayed_score[i]);  // add new score
      DrawDecimal(Screen, /*x=*/100, /*y=*/i << 3, /*color=*/i + 1, score);  // erase old score

      displayed_score[i] = score;
    }
  }
}

void DrawAll(gbyte* fb) {
  for (gbyte i = 0; i < NUM_SHIPS; i++) {
    struct body* p = TheShips + i;
    DrawShip(fb, p, i, gFALSE);
    struct body* m = TheMissiles + i;
    DrawShip(fb, m, i, gTRUE);
  }
}

void ProcessPacket(struct gamecast* chunk) {
  // struct broadcast_payload {
  // 	gbyte magic_aa;
  // 	gbyte ship_num;
  // 	word score;
  // 	struct body ship, missile;
  // };
  struct spacewar_msg* msg =
          (struct spacewar_msg*) chunk->payload;

  if (msg->magic_aa != 0xAA) return;
  gbyte n = msg->ship_num;
  if (n >= NUM_SHIPS) return;

  // TODO: is `->score` ignored?
  TheShips[n] = msg->ship;
  TheMissiles[n] = msg->missile;
}

void CheckIncomingPackets() {
    while (gALWAYS) {
        struct gamecast* chunk = gReceiveCast64();
        if (!chunk) {
            break;
        }
        ProcessPacket(chunk);
        gFree64(chunk);
    }
}

void setup() {
  gPMode1Screen(Screen, 0);
  ClearScreen(Screen, 0);
  gNetworkLog("Spacewar Running");

  gbyte my_num = mode == 0; // TODO: gScore.player (had problems)
  // struct body* my_missile = TheMissiles + my_num;

  // Draw a constellation around the Strong Gravity Zone.
  gPXOR(GraphicsAddr( W/2-W/8, H/2-H/8 ), 0x02); // 0x80);
  gPXOR(GraphicsAddr( W/2-W/8, H/2+H/8 ), 0x02); // 0x80);
  gPXOR(GraphicsAddr( W/2+W/8, H/2-H/8 ), 0x02);
  gPXOR(GraphicsAddr( W/2+W/8, H/2+H/8 ), 0x02);

  if (0) {  // background stars
    gword x = 4, y = 7;
    for (gword i = 0; i < 8; i++) {
      gPXOR(GraphicsAddr(x, y), 0x02);
      x += 47 + 64;
      y += 33;
      while (x > W) x -= W;
      while (y > H) y -= H;
    }
  }

  // Create Ships
  for (gword i = 0; i < NUM_SHIPS; i++) {
    volatile struct body* p = TheShips + i;
    p->x = 0x0800;
    p->y = ((i+1) << 12) + ((i+1) << 10);
    p->r = 30;  // 31+7*i;
    p->s = 0;  // 17+3*i;
    p->ttl = (i == my_num) ? 255 : 0;

    if (mode == 'S' || i == my_num) {
    	FireMissile(i);
    }

    displayed_score[i] = p->score;
    DrawDecimal(Screen, 100, i << 3, i + 1, p->score);
  }
}

void loop() {
  gbyte my_num = mode == 0; // TODO: gScore.player (had problems)
  struct body* my = TheShips + my_num;
  gword g = 0;
  gword embargo = 0;

  while (gALWAYS) {
    if (gTRUE || g & 1) {  // every time was to toucy.
      // Check keyboard.
      gbyte keys = RelevantKeysDown();

      if (keys & KEY_Z) gPMode1Screen(Screen, 0);

      if (keys & KEY_X) gPMode1Screen(Screen, 1);

      if (keys & KEY_Y) {
      	 my->direction = 8 ^ my->direction;
	     my->x = W/2 - 1 - my->x;
	     my->y = H/2 - 1 - my->y;
	     my->r = - my->r;
	     my->s = - my->s;
	     my->score--;
      }

      if (keys & KEY_LEFT) my->direction = (my->direction + 1) & 15;

      if (keys & KEY_RIGHT) my->direction = (my->direction - 1) & 15;

      if (keys & KEY_UP) {
#define THRUST 2  // was 3
        my->r += THRUST * AccelR[my->direction];
        my->s -= THRUST * AccelS[my->direction];
      }

      if (keys & KEY_SPACE && embargo < g) {
        FireMissile(my_num);
      }
    }

    // Advance the ships.
    // gbyte modeship = mode-'1';
    // if (modeship < NUM_SHIPS) {
    // TheShips[modeship].direction = direction;
    // }
    for (gword i = 0; i < NUM_SHIPS; i++) {
      AdvanceBody(TheShips + i, i, gTRUE);
      AdvanceBody(TheMissiles + i, i, gFALSE);
    }

    for (gword who = 0; who < NUM_SHIPS; who++) {
    	if (mode=='S' || who == my_num) {
	    struct body* who_missile = TheMissiles + who;
	    if (who_missile->ttl && (mode == 'S' || embargo < g)) {
	      gbool hit = DetectHits(who_missile, who);
	      if (hit) {
	      	if (mode != 'S') embargo = g + 55;  // about 5 seconds.
		Beep(30, 30);
	      }
	    }
      }
    }
    if (g == embargo - 1) {
      Beep(10, 90);
    }
  DRAW:
    DrawAll(Screen);
    DrawScores();

  WORK : {
    if ((mode != 'S') && ((g & 15) == 0)) {
      BroadcastShip(mode - '1');
    } else {
      Delay(1000);
    }

    if ((g & 3) == 0) {
      DrawSun(Screen, g);
    }
  }
  UNDRAW:
    DrawAll(Screen);

  CHECK:
    // Must UNDRAW before checking and depreciating.
    CheckIncomingPackets();

  DEPRECIATE:
    if (mode == 'S') {
      for (gbyte i = 0; i < NUM_SHIPS; i++) {
        TheShips[i].ttl = 255;  // keep everyone alive!
      }
    } else {
      for (gbyte i = 0; i < NUM_SHIPS; i++) {
        if (mode - '1' == i) {
          TheShips[i].ttl = 255;  // keep myself alive!
        } else {
          if (TheShips[i].ttl) TheShips[i].ttl--;  // depreciate others.
        }
      }
    }
    for (gbyte i = 0; i < NUM_SHIPS; i++) {
      if (TheMissiles[i].ttl) TheMissiles[i].ttl--;
    }

    g++;
    if (mode == 'S') {
	    switch (g & 255) {
	      case 20:	FireMissile(0); break;
	      case 90:	FireMissile(2); break;
	      case 140:	FireMissile(3); break;
	      case 120:	TheShips[0].direction = (TheShips[0].direction + 1) & 15;
	      case 190:	TheShips[3].direction = (TheShips[0].direction + 7) & 15;
	      case 40:	TheShips[2].direction = (TheShips[0].direction + 13) & 15;
	    }
    }
  }
}
