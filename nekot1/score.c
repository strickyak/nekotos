#include "nekot1/private.h"

// public:
byte N1NumberOfPlayers;
byte N1ThisPlayerNumber;
int N1PartialScores[N1_MAX_PLAYERS] MORE_DATA;
int N1TotalScores[N1_MAX_PLAYERS] MORE_DATA;

// private:
int OldPartialScores[N1_MAX_PLAYERS] MORE_DATA;

void DoPartialScores() {
    assert(Kern.in_game);
    assert(Kern.in_irq);
    byte dirty = false;
    byte np = N1NumberOfPlayers;
    {
        int* o = OldPartialScores;
        int* p = N1PartialScores;

        for (byte i = 0; i < np; i++) {
            if (*o++ != *p++) {
                dirty = true;
                break;
            }
        }
    }

    if (dirty) {
        N1SendClientPacket('S', (byte*) N1PartialScores, np);
        memcpy(OldPartialScores, N1PartialScores, sizeof N1PartialScores);
    }
}
