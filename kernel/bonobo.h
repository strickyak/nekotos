#ifndef _KERNEL_BONOBO_H_
#define _KERNEL_BONOBO_H_

void gBonoboStartNMI();
gbyte BonoboRecvChunkTry(gbyte* buf, gword n);
void BonoboSend(const gbyte* addr, gword n);
void Bonobo_Init();

#endif  // _KERNEL_BONOBO_H_
