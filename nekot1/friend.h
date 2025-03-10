#ifndef _NEKOT1_FRIEND_H_
#define _NEKOT1_FRIEND_H_

// Do not call gSendClientPacket directly.
// It is used by some public macros.
void gSendClientPacket(word p, char* pay, word size);

// gAfterMain3 is the actual function called by
// the macro gAfterMain(after_main).
void gAfterMain3(func after_main, word* final, word* final_startup);

#endif //  _NEKOT1_FRIEND_H_
