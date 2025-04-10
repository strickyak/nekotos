#include "kernel/private.h"

struct score gScore gZEROED;

void SendPartialScores() {
    if (0) gAssert(gKern.in_game);

    if (gScore.partial_dirty) {
        gScore.partial_dirty = gFALSE;

#if 0
PutChar('-');
PutChar('-');
PutChar('-');
PutChar('[');
PutChar('&');
PutChar(']');
NowSwitchToChatScreen();
#endif
        xSendControlPacket('S', (gbyte*) gScore.partial_scores, gScore.number_of_players<<1);
    }
}
