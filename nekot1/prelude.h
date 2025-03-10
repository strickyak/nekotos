#ifndef _NEKOT1_PRELUDE_H_
#define _NEKOT1_PRELUDE_H_

#define Cons ((gbyte*)0x0200)

#define InitialStack 0x01FE // going backwards.  2-gbyte canary after stack.

#define Pia0PortA     0xFF00u
#define Pia0PortB     0xFF02u
#define Pia1PortA     0xFF20u
#define Pia1PortB     0xFF22u

#define IRQVEC_COCO12 0x010Cu
#define IRQVEC_COCO3  0xFEF7u

#define JMP_Extended  (gbyte)0x7E

void MemCopy(gbyte *dest, const gbyte *src, gword count);
void MemSet(gbyte* dest, gbyte value, gword count);
void* memset(void* dest, int value, gword count);

void gFatal(const char* s, gword arg);

#endif // _NEKOT1_PRELUDE_H_
