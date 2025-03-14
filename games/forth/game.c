#include "nekot1/public.h"

gSCREEN(TextScreen, 2);

#include "games/forth/forth.h"
#include "games/forth/platform.c"

#include "games/forth/dictionary.c"
#include "games/forth/execute.c"
#include "games/forth/find.c"
#include "games/forth/handle_compile.c"
#include "games/forth/initdict.c"
#include "games/forth/input.c"
#include "games/forth/number.c"
#include "games/forth/output.c"
#include "games/forth/parse.c"
#include "games/forth/stack.c"
#include "games/forth/walk.c"

void setup() {
    gTextScreen(TextScreen, 0);

    Text_Init();
    Printf("-- Hello FORTH -- ");
#if 0
    for (int c = 0; c < 10; c++) {
        PutChar(':');
        PutDec(c);
        PutChar(':');
        int wut = getchar();
        PutDec(wut);
    }
#endif

    init_dictionary();
    rclear();
    clear();
    set_input(tib, TIBSIZE, TIB_ID);
}

void loop() {
    if(refill()) {
        char *word_adr;
        cell word_len;

        while ((word_len = parse_word(&word_adr)) != 0) {
            handle_word(word_adr, word_len);
        }
    } else {
        gFatal("cannot refill", 0);
    }
}
