#ifndef _NEKOT1_PRELUDE_H_
#define _NEKOT1_PRELUDE_H_

// TODO: Get rid of prelude.[ch]

#define Cons ((gbyte*)0x0200)

#define Pia0PortA     0xFF00u
#define Pia0PortB     0xFF02u
#define Pia1PortA     0xFF20u
#define Pia1PortB     0xFF22u

#define IRQVEC_COCO12 0x010Cu
#define IRQVEC_COCO3  0xFEF7u

#define OPCODE_JMP_Extended  (gbyte)0x7E

typedef void (*gfunc)(void);

#endif // _NEKOT1_PRELUDE_H_
