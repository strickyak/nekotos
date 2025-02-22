#ifndef _PRELUDE_H_
#define _PRELUDE_H_

typedef unsigned char bool;
typedef unsigned char byte;
typedef volatile byte *vptr;
typedef unsigned int uint;
typedef unsigned int word;
typedef unsigned int size_t;
typedef void (*func)(void);
typedef union wordorbytes {
    word w;
    byte b[2];
} wob;

#define true ((bool)1)
#define false ((bool)0)

#define Peek1(ADDR) (*(vptr)(uint)(ADDR))
#define Poke1(ADDR,VALUE) (*(vptr)(uint)(ADDR) = (byte)(VALUE))

#define Peek2(ADDR) (*(volatile uint*)(uint)(ADDR))
#define Poke2(ADDR,VALUE) (*(volatile uint*)(uint)(ADDR) = (uint)(VALUE))

#define PAND(ADDR, X) ((*(vptr)(size_t)(ADDR)) &= (byte)(X))
#define POR(ADDR, X) ((*(vptr)(size_t)(ADDR)) |= (byte)(X))
#define PXOR(ADDR, X) ((*(vptr)(size_t)(ADDR)) ^= (byte)(X))

#define Cons ((vptr)0x0200)
#define Disp ((vptr)0x0400)
#define InitialStack 0x01FE // going backwards.  2-byte canary after stack.
#define KernelBegin 0x1002  // two byte canary before code.
#define KernelEntry 0x1002

#define Pia0PortA     0xFF00u
#define Pia0PortB     0xFF02u

#define IRQVEC_COCO12 0x010Cu
#define IRQVEC_COCO3  0xFEF7u

#define JMP_Extended  (byte)0x7E

void MemCopy(byte *dest, const byte *src, uint count);
void MemSet(byte* dest, byte value, size_t n);

void Fatal(const char* s, uint arg);

#define INHIBIT_IRQ() asm volatile("  orcc #$10")
#define ALLOW_IRQ()   asm volatile("  andcc #^$10")

#define assert(COND) if (!(COND)) Fatal(__FILE__, __LINE__)

#define STARTUP_DATA   __attribute__ ((section (".data.startup")))
#define KERN_FINAL     __attribute__ ((section (".final.kern")))
#define STARTUP_FINAL  __attribute__ ((section (".final.startup")))
#define MORE_DATA      __attribute__ ((section (".data.more")))

#define DEFINE_SCREEN(G,N)   extern volatile byte G[N]

#endif // _PRELUDE_H_
