// platform.c
// for NEKOT OS on 6809

#include "forth.h"

#include "games/lib8/platform-keyboard.c"
#include "games/lib8/platform-text.c"

FILE stdin[1] = { { 0, }, };
FILE stdout[1] = { { 1, }, };
FILE stderr[1] = { { 2, }, };

void fatal(const char* s, gword arg) {
    printf("** FATAL(%d)  %s **\n", arg, s);
    exit(7);
}
void abort() {
    printf("** ABORT. \n");
    exit(13);
}
void exit(int a) {
    printf("** EXIT %d.  (HIT BREAK) \n", a);
    while (gKern.always_true) {
        gPoke2(0x3FE0, a++);
    }
    while (true) {}
}
int feof(FILE* f) { return 0; }
void fflush(FILE* f) {}
int fgetc(FILE* f) {
    return getchar();
}
int fputc(int ch, FILE* f) {
    PutChar(ch);
    return 0;
}
int isspace(int ch) { return ch <= 32; }
