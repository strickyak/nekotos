#ifndef _KERNEL_BONOBO_H_
#define _KERNEL_BONOBO_H_

void gBonoboStartRepeatingNMI(gword micros);
gbyte BonoboRecvChunkTry(gbyte* buf, gword n);
void BonoboSend(const gbyte* addr, gword n);
void Bonobo_Init();

#endif  // _KERNEL_BONOBO_H_
