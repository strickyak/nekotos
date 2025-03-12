#include "nekot1/private.h"

struct score gScore gZEROED;

void SendPartialScores() {
    gAssert(gKern.in_game);
    gAssert(gKern.in_irq);
    gbyte dirty = gFALSE;
    gbyte np = gScore.number_of_players;
    {
        int* o = gScore.old_partials;
        int* p = gScore.partials;

        for (gbyte i = 0; i < np; i++) {
            if (*o++ != *p++) {
                dirty = gTRUE;
                break;
            }
        }
    }

    if (dirty) {
        xSendControlPacket('S', (gbyte*) gScore.partials, np<<1);
        memcpy(gScore.old_partials, gScore.partials, sizeof gScore.partials);
    }
}
