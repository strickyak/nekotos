#ifndef _PRELUDE_H_
#define _PRELUDE_H_

typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned int uint;
typedef void (*func)(void);

#define true ((bool)1)
#define false ((bool)0)

#define Peek1(ADDR) (*(volatile byte*)(uint)(ADDR))
#define Poke1(ADDR,VALUE) (*(volatile byte*)(uint)(ADDR) = (VALUE))

#define Peek2(ADDR) (*(volatile uint*)(uint)(ADDR))
#define Poke2(ADDR,VALUE) (*(volatile uint*)(uint)(ADDR) = (VALUE))

const uint Pia0PortA = 0xFF00;
const uint Pia0PortB = 0xFF02;

const uint IRQVEC = 0x010C;

const byte JMP_Extended = 0x7E;

struct prelude {
};

void memcpy(byte *dest, byte *src, uint count);

#endif // _PRELUDE_H_
