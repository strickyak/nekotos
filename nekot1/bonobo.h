#ifndef _NEKOT1_BONOBO_H_
#define _NEKOT1_BONOBO_H_

gbyte BonoboRecvChunkTry(gbyte* buf, gword n);
void BonoboSend(const gbyte* addr, gword n);
void Bonobo_Init();

#endif // _NEKOT1_BONOBO_H_
