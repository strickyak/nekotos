#ifndef _g_PRELUDE_H_
#define _g_PRELUDE_H_

#define Cons ((byte*)0x0200)

#define InitialStack 0x01FE // going backwards.  2-byte canary after stack.

#define Pia0PortA     0xFF00u
#define Pia0PortB     0xFF02u
#define Pia1PortA     0xFF20u
#define Pia1PortB     0xFF22u

#define IRQVEC_COCO12 0x010Cu
#define IRQVEC_COCO3  0xFEF7u

#define JMP_Extended  (byte)0x7E

void MemCopy(byte *dest, const byte *src, word count);
void MemSet(byte* dest, byte value, word count);
void* memset(void* dest, int value, word count);

void Fatal(const char* s, word arg);

#endif // _g_PRELUDE_H_
