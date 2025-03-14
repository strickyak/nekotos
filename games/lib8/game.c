gSCREEN(TextScreen, 2);

#include "games/lib8/platform-keyboard.h"
#include "games/lib8/platform-text.h"

#include "games/lib8/platform-keyboard.c"
#include "games/lib8/platform-text.c"

void setup() {
    gTextScreen(TextScreen, 0);

    Text_Init();
    Printf("Hello LIB8\n");
}

void loop() {
    int c = getchar();
    if (c) {
        // PutDec(c);
        PutChar(c);
    }

    ++ TextScreen[511];
}
