#include "kernel/public.h"

gSCREEN(D, 2);  // D for Semigraphics Display

volatile gbyte TRUE = 1;

#define H 32  // must be a power of 2
#define W 64  // must be a power of 2
#define MODH(A) ((H-1)&(A))
#define MODW(A) ((W-1)&(A))

gbyte board[2][W][H] gZEROED;

void ComputeLife(gbyte in[W][H], gbyte out[W][H]) {
    for (gword y = 0; y < H; y++) {
        for (gword x = 0; x < W; x++) {
            gbyte n =
                in[MODW(x-1)][MODH(y-1)] +
                in[MODW(x-1)][y] +
                in[MODW(x-1)][MODH(y+1)] +
                in[x][MODH(y-1)] +
                in[x][MODH(y+1)] +
                in[MODW(x+1)][MODH(y-1)] +
                in[MODW(x+1)][y] +
                in[MODW(x+1)][MODH(y+1)];
            if (in[x][y]) {
                out[x][y] = (n==2 || n==3);
            } else {
                out[x][y] = (n==3);
            }
        }
    }
}

void DisplayLife(gbyte in[W][H]) {
    volatile gbyte* p = (volatile gbyte*)D;
    for (gword y = 0; y < H; y+=2) {
        for (gword x = 0; x < W; x+=2) {
            *p = '#';
            gbyte z = 0x80;
            if (in[x][y]) z += 0x08;
            if (in[x+1][y]) z += 0x04;
            if (in[x][y+1]) z += 0x02;
            if (in[x+1][y+1]) z += 0x01;
            *p++ = z;
        }
    }
}

volatile gword generation;
void loop() {
            DisplayLife(board[0]);
            ComputeLife(board[0], board[1]);
            ++generation;

            DisplayLife(board[1]);
            ComputeLife(board[1], board[0]);
            ++generation;
}

void setup() {
    gTextScreen(D, 1);
    gNetworkLog("hello life");

    for (gword y = 0; y < H; y++) {
        for (gword x = 0; x < W; x++) {
            board[0][x][y] = board[1][x][y] = 0;
        }
    }

    for (gword y = 2; y < H*3/4; y++) {
        for (gword x = 2; x < W*3/4; x++) {
            gbyte z = (gbyte)x & (gbyte)y;
            gbyte c = gMono.ticks + gMono.decis + gMono.seconds;
            gbyte b = 1;
            for (gword i = 0; i<8; i++) {
                if ((b&z) != 0) c++;
                b <<= 1;
            }
            if ((c & 3) == 1) board[0][x][y] = 1;
        }
    }
}
