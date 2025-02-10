// DECLARE_FRAMEBUFFER(Text, 2)
// DECLARE_FRAMEBUFFER(Graf, 12)

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

void FONT_Wrapper() {
	// ZX Times font by DamienG https://damieng.com
    asm volatile(";;; \n"
    "_FONT:            \n"
	"  fcb $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $38,$38,$38,$10,$10,$00,$38,$00,$00,$00,$00,$00 \n"
	"  fcb $6c,$6c,$48,$00,$00,$00,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$14,$7e,$28,$28,$fc,$50,$00,$00,$00,$00,$00 \n"
	"  fcb $3e,$6a,$78,$3c,$1e,$56,$7c,$10,$00,$00,$00,$00 \n"
	"  fcb $72,$da,$74,$18,$2e,$5b,$4e,$00,$00,$00,$00,$00 \n"
	"  fcb $30,$68,$68,$77,$da,$dc,$76,$00,$00,$00,$00,$00 \n"
	"  fcb $18,$18,$10,$00,$00,$00,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $08,$10,$30,$30,$30,$30,$10,$08,$00,$00,$00,$00 \n"
	"  fcb $20,$10,$18,$18,$18,$18,$10,$20,$00,$00,$00,$00 \n"
	"  fcb $00,$10,$54,$38,$54,$10,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$18,$18,$7e,$18,$18,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$00,$00,$00,$00,$18,$18,$10,$00,$00,$00,$00 \n"
	"  fcb $00,$00,$00,$7c,$00,$00,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$00,$00,$00,$00,$18,$18,$00,$00,$00,$00,$00 \n"
	"  fcb $06,$0c,$0c,$18,$18,$30,$30,$60,$00,$00,$00,$00 \n"
	"  fcb $18,$24,$66,$66,$66,$24,$18,$00,$00,$00,$00,$00 \n"
	"  fcb $18,$38,$18,$18,$18,$18,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $3c,$66,$06,$0c,$18,$32,$7e,$00,$00,$00,$00,$00 \n"
	"  fcb $3c,$66,$06,$1c,$06,$66,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $0c,$1c,$2c,$4c,$7e,$0c,$0c,$00,$00,$00,$00,$00 \n"
	"  fcb $3e,$60,$70,$0c,$06,$46,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $1c,$30,$60,$7c,$66,$66,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $7e,$46,$06,$0c,$0c,$18,$18,$00,$00,$00,$00,$00 \n"
	"  fcb $3c,$62,$72,$3c,$4e,$46,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $3c,$66,$66,$3e,$06,$0c,$38,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$00,$18,$18,$00,$18,$18,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$00,$18,$18,$00,$18,$18,$10,$00,$00,$00,$00 \n"
	"  fcb $00,$06,$1c,$70,$1c,$06,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$00,$7c,$00,$7c,$00,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$60,$38,$0e,$38,$60,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $3c,$66,$06,$0c,$10,$00,$30,$00,$00,$00,$00,$00 \n"
	"  fcb $38,$44,$de,$e6,$e6,$de,$40,$3c,$00,$00,$00,$00 \n"
	"  fcb $18,$18,$2c,$2c,$7e,$46,$ef,$00,$00,$00,$00,$00 \n"
	"  fcb $fc,$66,$66,$7c,$66,$66,$fc,$00,$00,$00,$00,$00 \n"
	"  fcb $3a,$66,$c0,$c0,$c0,$62,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $f8,$6c,$66,$66,$66,$6c,$f8,$00,$00,$00,$00,$00 \n"
	"  fcb $fe,$62,$68,$78,$68,$62,$fc,$00,$00,$00,$00,$00 \n"
	"  fcb $fe,$62,$68,$78,$68,$60,$f0,$00,$00,$00,$00,$00 \n"
	"  fcb $34,$6c,$c0,$ce,$c4,$64,$38,$00,$00,$00,$00,$00 \n"
	"  fcb $f7,$66,$66,$7e,$66,$66,$ef,$00,$00,$00,$00,$00 \n"
	"  fcb $3c,$18,$18,$18,$18,$18,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $1e,$0c,$0c,$0c,$6c,$68,$30,$00,$00,$00,$00,$00 \n"
	"  fcb $ee,$64,$68,$70,$78,$6c,$ee,$00,$00,$00,$00,$00 \n"
	"  fcb $f0,$60,$60,$60,$60,$62,$fc,$00,$00,$00,$00,$00 \n"
	"  fcb $f7,$76,$76,$5a,$5a,$5a,$e7,$00,$00,$00,$00,$00 \n"
	"  fcb $ce,$64,$74,$7c,$5c,$4c,$e4,$00,$00,$00,$00,$00 \n"
	"  fcb $38,$6c,$c6,$c6,$c6,$6c,$38,$00,$00,$00,$00,$00 \n"
	"  fcb $fc,$66,$66,$64,$78,$60,$f0,$00,$00,$00,$00,$00 \n"
	"  fcb $38,$6c,$c6,$c6,$c6,$6c,$38,$0c,$00,$00,$00,$00 \n"
	"  fcb $fc,$66,$66,$7c,$6c,$66,$f7,$00,$00,$00,$00,$00 \n"
	"  fcb $3a,$66,$70,$3c,$0e,$66,$5c,$00,$00,$00,$00,$00 \n"
	"  fcb $7e,$5a,$18,$18,$18,$18,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $e7,$62,$62,$62,$62,$62,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $e7,$62,$62,$34,$34,$18,$18,$00,$00,$00,$00,$00 \n"
	"  fcb $f7,$6a,$6a,$7e,$7e,$76,$62,$00,$00,$00,$00,$00 \n"
	"  fcb $e6,$64,$38,$38,$38,$4c,$ce,$00,$00,$00,$00,$00 \n"
	"  fcb $f7,$62,$34,$34,$18,$18,$3c,$00,$00,$00,$00,$00 \n"
	"  fcb $7e,$46,$0c,$18,$30,$62,$7e,$00,$00,$00,$00,$00 \n"
	"  fcb $3c,$30,$30,$30,$30,$30,$30,$3c,$00,$00,$00,$00 \n"
	"  fcb $60,$30,$30,$18,$18,$0c,$0c,$06,$00,$00,$00,$00 \n"
	"  fcb $3c,$0c,$0c,$0c,$0c,$0c,$0c,$3c,$00,$00,$00,$00 \n"
	"  fcb $10,$38,$6c,$44,$00,$00,$00,$00,$00,$00,$00,$00 \n"
	"  fcb $00,$00,$00,$00,$00,$00,$00,$ff,$00,$00,$00,$00 \n"
    );
}

/////////////////////////////////////////////////////

typedef void (*SpotDrawer)(word fb, byte x, byte y, byte color);

void DrawSpot(word fb, byte x, byte y, byte color) {
  byte xshift = x & 3;  // mod 4
  byte xdist = x >> 2;  // div 4
  word addr = fb + xdist + ((word)y << 5);
  byte b = Peek1(addr);
  byte bitshift = (3 - xshift) << 1;
  byte mask = ~(3 << bitshift);
  b = (b & mask) | (color << bitshift);
  Poke1(addr, b);
}
void DrawSpotXor(word fb, byte x, byte y, byte color) {
  byte xshift = x & 3;  // mod 4
  byte xdist = x >> 2;  // div 4
  word addr = fb + xdist + ((word)y << 5);
  PXOR(addr, (color << ((3 - xshift) << 1)));
}
void DrawHorz(word fb, byte x, byte y, byte color, byte len, SpotDrawer spot) {
    byte last = x + len;
    for (byte i = x; i <= last; i++) {
        spot(fb, i, y, color);
    }
}
void DrawVirt(word fb, byte x, byte y, byte color, byte len, SpotDrawer spot) {
    byte last = y + len;
    for (byte i = y; i <= last; i++) {
        spot(fb, x, i, color);
    }
}

#define INHIBIT_IRQ() asm volatile("  orcc #$10")
#define ALLOW_IRQ()   asm volatile("  andcc #^$10")

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

void ClearGraf(byte color) {
    color &= 3;
    wob c;
    c.b[0] = c.b[1] = color | (color<<2) | (color<<4) | (color<<6);
    for (word i = Graf; i < Graf + 3*1024; i+=2) {
        Poke2(i, c.w);
    }
}

void WaitFor60HzTick() {
    byte t = Peek1(&Real.ticks);
    while (Peek1(&Real.ticks) == t) {}
}
void WaitForKeyPressArrowsAnd0To7() {
   wob w;
   do { w = ScanArrowsAnd0To7(); }
   while (w.w == 0);
}

extern byte FONT[];
void DrawChar(char ch, byte x, byte y, byte color) {
    word c = ch - 32;
    // word p = FONT + 12*c;
    word p = (word)FONT + (c<<3) + (c<<2);
    Printf("( %x %x %x);\n", ch, c, p);
    for (word i = 0; i < 8; i++) {
        byte bits = Peek1(p++);
        Printf("%x.", bits);
        byte probe = 0x80u;
        for (byte j = 0; j < 8; j++) {
            if (bits & probe)
                DrawSpotXor(Graf, x+j, y+i, Blue0);
            probe >>= 1;
        }
    }
}

void Spacewar_Main() {
    Poke2(0, FONT_Wrapper);
    Poke2(0, DrawChar);
    Poke2(0, DrawSpotXor);

    Vdg_GamePMode1(Graf, 0);
    ALLOW_IRQ();

    while (true) {

        ClearGraf(Yellow0);

        {
            byte x = 0;
            for (const char* s = "HELLO!"; *s; s++) {
                DrawChar(*s, x, x, Blue0);
                x += 8;
                WaitFor60HzTick();
            }
        }
        if (0) {
            byte x = 0;
            for (const char* s = "HELLO!"; *s; s++) {
                DrawChar(*s, x, x, Red0);
                x += 8;
                WaitFor60HzTick();
            }
        }
    }
    Vdg_GameText(0x0200, 0);
    while (1) {}
}
