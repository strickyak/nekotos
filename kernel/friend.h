#ifndef _KERNEL_FRIEND_H_
#define _KERNEL_FRIEND_H_

// Do not call xSendControlPacket directly.
// It is used by some public macros.
void xSendControlPacket(gword p, const gbyte* pay, gword size);

#endif  //  _KERNEL_FRIEND_H_
