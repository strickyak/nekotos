#ifndef _NEKOT1_FRIEND_H_
#define _NEKOT1_FRIEND_H_

// Do not call xSendControlPacket directly.
// It is used by some public macros.
void xSendControlPacket(gword p, const gbyte* pay, gword size);

#endif //  _NEKOT1_FRIEND_H_
