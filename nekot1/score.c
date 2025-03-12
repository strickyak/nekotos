#include "nekot1/private.h"

// public:
gbyte gNumberOfPlayers;
gbyte gThisPlayerNumber;
int gPartialScores[gMAX_PLAYERS] gZEROED;
int gTotalScores[gMAX_PLAYERS] gZEROED;

// private:
int OldPartialScores[gMAX_PLAYERS] gZEROED;

void DoPartialScores() {
    gAssert(gKern.in_game);
    gAssert(gKern.in_irq);
    gbyte dirty = gFALSE;
    gbyte np = gNumberOfPlayers;
    {
        int* o = OldPartialScores;
        int* p = gPartialScores;

        for (gbyte i = 0; i < np; i++) {
            if (*o++ != *p++) {
                dirty = gTRUE;
                break;
            }
        }
    }

    if (dirty) {
        xSendControlPacket('S', (gbyte*) gPartialScores, np);
        memcpy(OldPartialScores, gPartialScores, sizeof gPartialScores);
    }
}
