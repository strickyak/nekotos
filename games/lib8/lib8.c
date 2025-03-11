#include "nekot1/public.h"

gSCREEN(TextScreen, 2);

#include "games/lib8/platform-keyboard.h"
#include "games/lib8/platform-text.h"

#include "games/lib8/platform-keyboard.c"
#include "games/lib8/platform-text.c"

int main() {
    gBeginMain();
    gTextScreen(TextScreen, 0);

    Text_Init();
    Printf("Hello LIB8\n");

    while (gKern.always_true) {
        int c = getchar();
        PutChar(c);
    }

    while (gKern.always_true) {
        ++ TextScreen[511];
    }
}
