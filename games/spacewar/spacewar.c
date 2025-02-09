// DECLARE_FRAMEBUFFER(Text, 2)
// DECLARE_FRAMEBUFFER(Graf, 12)

#define PXOR(ADDR, X) ((*(volatile byte*)(size_t)(ADDR)) ^= (byte)(X))

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

/////////////////////////////////////////////////////

// gcc6809 -f'whole-program' doesn't like libgcc runtime library calls.
// So here are ShiftRight and ArithShiftRight tht avoid library calls.
word ShiftLeft(word x, byte count) {
  for (byte i = 0; i < count; i++) {
    x <<= 1;
  }
  return x;
}

byte ByteShiftLeft(byte x, byte count) {
  for (byte i = 0; i < count; i++) {
    x <<= 1;
  }
  return x;
}

int ShiftRight(int x, byte count) {
  word z = (word)x;
  for (byte i = 0; i < count; i++) {
    z >>= 1;
  }
  return (int)z;
}

int ArithShiftRight(int x, byte count) {
  word z = (word)x;
  for (byte i = 0; i < count; i++) {
    z >>= 1;
    if (x & 0x8000) z |= 0x8000;
  }
  return (int)z;
}

/////////////////////////////////////////////////////

void DrawSpot(word fb, byte x, byte y, byte color) {
  byte xshift = x & 3;  // mod 4
  byte xdist = x >> 2;  // div 4
  word addr = fb + xdist + ((word)y << 5);
  PXOR(addr, ByteShiftLeft(color, (3 - xshift) << 1));
}
void DrawHorz(word fb, byte x, byte y, byte color, byte len) {
    byte last = x + len;
    for (byte i = x; i <= last; i++) {
        DrawSpot(fb, i, y, color);
    }
}
void DrawVirt(word fb, byte x, byte y, byte color, byte len) {
    byte last = y + len;
    for (byte i = y; i <= last; i++) {
        DrawSpot(fb, x, i, color);
    }
}

#define INHIBIT_IRQ() asm volatile("  orcc #$10")
#define ALLOW_IRQ()   asm volatile("  andcc #^$10")

typedef union wordorbytes {
    word w;
    byte b[2];
} wob;

wob ScanArrowsAnd0To7() {
    union wordorbytes z;
    z.w = 0;
    INHIBIT_IRQ();
    for (byte bit=1; bit; bit<<=1) {
        Poke1(Pia0PortB, ~bit);
        byte sense = Peek1(Pia0PortA);
        if ((sense & 0x08) == 0) z.b[0] |= bit;
        if ((sense & 0x10) == 0) z.b[1] |= bit;
    }
    ALLOW_IRQ();
    return z;
}

void ClearGraf() {
    for (word i = Graf; i < Graf + 3*1024; i+=2) {
        Poke2(i, 0);
    }
}

void Spacewar_Main() {
    Vdg_GamePMode1(Graf, 0);
    ALLOW_IRQ();
    ClearGraf();

    volatile byte* p = (byte*) 0x300;

    for (uint a = 0; a < 96; a++) {
        for (uint b = 0; b < 128; b++) {
#if 0
#if 0
            // Wait 1/60 sec
            byte t = Peek(&Real.ticks);
            while (Peek(&Real.ticks) == t) {}
#else
            wob w;
            do { w = ScanArrowsAnd0To7(); }
            while (w.w == 0);
#endif
#endif
            DrawSpot(Graf, b, a, Red0);
        }
    }

#if 0
    while (true) {
        p[0]+=1;
        if (!p[0]) {
            p[1]+=1;
            if (!p[1]) {
                p[2]+=1;
            }
        }
    }
#endif
}
