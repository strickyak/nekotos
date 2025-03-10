#include "nekot1/private.h"

// public:
byte gNumberOfPlayers;
byte gThisPlayerNumber;
int gPartialScores[gMAX_PLAYERS] MORE_DATA;
int gTotalScores[gMAX_PLAYERS] MORE_DATA;

// private:
int OldPartialScores[gMAX_PLAYERS] MORE_DATA;

void DoPartialScores() {
    assert(Kern.in_game);
    assert(Kern.in_irq);
    byte dirty = false;
    byte np = gNumberOfPlayers;
    {
        int* o = OldPartialScores;
        int* p = gPartialScores;

        for (byte i = 0; i < np; i++) {
            if (*o++ != *p++) {
                dirty = true;
                break;
            }
        }
    }

    if (dirty) {
        gSendClientPacket('S', (byte*) gPartialScores, np);
        memcpy(OldPartialScores, gPartialScores, sizeof gPartialScores);
    }
}
