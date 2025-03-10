#include "nekot1/private.h"

// public:
gbyte gNumberOfPlayers;
gbyte gThisPlayerNumber;
int gPartialScores[gMAX_PLAYERS] MORE_DATA;
int gTotalScores[gMAX_PLAYERS] MORE_DATA;

// private:
int OldPartialScores[gMAX_PLAYERS] MORE_DATA;

void DoPartialScores() {
    assert(Kern.in_game);
    assert(Kern.in_irq);
    gbyte dirty = false;
    gbyte np = gNumberOfPlayers;
    {
        int* o = OldPartialScores;
        int* p = gPartialScores;

        for (gbyte i = 0; i < np; i++) {
            if (*o++ != *p++) {
                dirty = true;
                break;
            }
        }
    }

    if (dirty) {
        gSendClientPacket('S', (gbyte*) gPartialScores, np);
        memcpy(OldPartialScores, gPartialScores, sizeof gPartialScores);
    }
}
