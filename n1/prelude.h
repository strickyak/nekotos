#ifndef _N1_PRELUDE_H_
#define _N1_PRELUDE_H_

typedef volatile byte *vptr;

#define PAND(ADDR, X) ((*(vptr)(word)(ADDR)) &= (byte)(X))
#define POR(ADDR, X) ((*(vptr)(word)(ADDR)) |= (byte)(X))
#define PXOR(ADDR, X) ((*(vptr)(word)(ADDR)) ^= (byte)(X))

#define Cons ((vptr)0x0200)

#define InitialStack 0x01FE // going backwards.  2-byte canary after stack.
//#define KernelBegin 0x0502  // two byte canary before code.
//#define KernelEntry 0x0502

#define Pia0PortA     0xFF00u
#define Pia0PortB     0xFF02u

#define IRQVEC_COCO12 0x010Cu
#define IRQVEC_COCO3  0xFEF7u

#define JMP_Extended  (byte)0x7E

void MemCopy(byte *dest, const byte *src, word count);
void MemSet(byte* dest, byte value, word n);

void Fatal(const char* s, word arg);

#define STARTUP_DATA   __attribute__ ((section (".data.startup")))
#define KERN_FINAL     __attribute__ ((section (".final.kern")))
#define STARTUP_FINAL  __attribute__ ((section (".final.startup")))
#define MORE_DATA      __attribute__ ((section (".data.more")))

#endif // _N1_PRELUDE_H_
