#define N 16

struct invader {
    gbyte x, y;
    gbyte r, s;
    gbyte color;
    gbyte* sprite;
} Inv[N] gZEROED;

gbyte Colors[] = { 0x55, 0xAA, 0xFF };

char One[] =
    "++++++++++++"
    ".....XX....."
    "...XXXXXX..."
    "..XXXXXXXX.."
    ".XX..XX..XX."
    ".XXXXXXXXXX."
    "...XX..XX..."
    "..X..XX..X.."
    "XX.XX..XX.XX"
    ;

char Two[] =
    "++++++++++++"
    "..X......X.."
    "X..X....X..X"
    "X.XXXXXXXX.X"
    "XXX XXXX XXX"
    "XXXXXXXXXXXX"
    ".XXXXXXXXXX."
    "..X......X.."
    ".X........X."
    "            "
    ;

char Three[] =
    "++++++++++++"
    "..X......X.."
    "...X....X..."
    "..XXXXXXXX.."
    ".XX XXXX XX."
    "XXXXXXXXXXXX"
    "X.XXXXXXXX.X"
    "X.X......X.X"
    "...XX..XX..."
    "            "
    ;

char Four[] =
    "++++++++++++"
    "....XXXX...."
    ".XXXXXXXXXX."
    "XXXXXXXXXXXX"
    "XXX  XX  XXX"
    "XXXXXXXXXXXX"
    "   XXXXXX   "
    " XXX XX XXX "
    "            "
    ;

void CompileSprite(const char* s, gbyte* b) {
    s += 12; // right now, always 12 wide.

    for (gword y = 0; y < 8; y++) {
        for (gword x = 0; x < 12; x+=4) {
            gbyte bits = 0;
            if (*s++ > '?') bits |= 0xC0;
            if (*s++ > '?') bits |= 0x30;
            if (*s++ > '?') bits |= 0x0C;
            if (*s++ > '?') bits |= 0x03;
            *b++ = bits;
        }
    }
}

gbyte inv1[3*8] gZEROED;
gbyte inv2[3*8] gZEROED;
gbyte inv3[3*8] gZEROED;
gbyte inv4[3*8] gZEROED;

void DrawAt(gbyte* sprite, gbyte x, gbyte y, gbyte color) {
#if 1
    color = 0xAA;
#endif
    gbyte* p = G + x + ((gword)y<<5);
    for (gbyte i = 0; i < 8; i++) {
        p[0] = *sprite++ & color;
        p[1] = *sprite++ & color;
        p[2] = *sprite++ & color;
        p += 32;
    }
}
void ClearAt(gbyte x, gbyte y) {
    gbyte* p = G + x + ((gword)y<<5);
    for (gbyte i = 0; i < 8; i++) {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p += 32;
    }
}

void InvaderLoop() {
    for (gbyte i = 0; i < N; i++) {
        struct invader* q = &Inv[i];
        ClearAt(q->x, q->y);
        q->x += q->r;
        q->y += q->s;
        q->x = 31 & q->x;
        q->y = (q->y > 192) ? q->y + 96 : q->y;
        q->y = (q->y > 96) ? q->y - 96 : q->y;
        DrawAt(q->sprite, q->x, q->y, q->color);
    }
}

void InvaderSetup() {
    CompileSprite(One, inv1);
    CompileSprite(Two, inv2);
    CompileSprite(Three, inv3);
    CompileSprite(Four, inv4);

    gbyte x0 = 3, y0 = 8;
    gbyte jx = 0;
    gbyte jy = 0;
    for (gbyte i = 0; i < N; i++) {
        struct invader* q = &Inv[i];
        q->x = x0;
        q->y = y0;
        x0 = 31 & (x0+23);
        y0 = (y0+11);
        q->y = (q->y > 192) ? q->y + 96 : q->y;
        q->y = (q->y > 96) ? q->y - 96 : q->y;

        q->color = Colors[jx];
        jx = (jx==2) ? 0 : jx+1;
        if (i&2) {
            jy = (jy==2) ? 0 : jy+1;
        }
        q->r = jx-1;
        q->s = (jx==1 && jy==1) ? 2 : jy-1;

        switch (i&3) {
        case 0: q->sprite = inv1; break;
        case 1: q->sprite = inv2; break;
        case 2: q->sprite = inv3; break;
        case 3: q->sprite = inv4; break;
        }
    }
}
