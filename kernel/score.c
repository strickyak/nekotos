#include "kernel/private.h"

struct score gScore gZEROED;

void SendPartialScores() {
    gPoke2(0, SendPartialScores);

    return;  // for now, skip this.  why buggy?

    gAssert(gKern.in_game);
    gAssert(gKern.in_irq);
    gbyte dirty = gFALSE;
    gbyte np = gScore.number_of_players;
    if (!np) return;

    PutStr("SPS/");
    PutDec(np);
        //int* o = gScore.old_partials;
        //int* p = gScore.partials;
    {

    PutChar('@');
    PutDec((gword)gScore.old_partials);
    PutChar('p');
    PutDec((gword)gScore.partials);

        for (gbyte i = 0; i < np; i++) {
    PutChar('#');
    PutDec(i);
    PutChar(',');
    PutDec(gScore.old_partials[i]);
    PutChar(',');
    PutDec(gScore.partials[i]);
    PutChar(';');
            // if (*o != *p) //
            if (gScore.old_partials[i] != gScore.partials[i]) {
    PutChar('!');
                dirty = gTRUE;
                break;
            }
            //++o;
            //++p;
        }
    }

    if (dirty) {
        xSendControlPacket('S', (gbyte*) gScore.partials, np<<1);
        memcpy(gScore.old_partials, gScore.partials, sizeof gScore.partials);
        gFatal("TMP", sizeof gScore.partials);
    }
}
