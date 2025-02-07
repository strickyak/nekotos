#line 1 "prelude.h"
#ifndef _PRELUDE_H_
#define _PRELUDE_H_

typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned int word;
typedef unsigned int size_t;
typedef void (*func)(void);

#define true ((bool)1)
#define false ((bool)0)

#define Peek1(ADDR) (*(volatile byte*)(uint)(ADDR))
#define Poke1(ADDR,VALUE) (*(volatile byte*)(uint)(ADDR) = (byte)(VALUE))

#define Peek2(ADDR) (*(volatile uint*)(uint)(ADDR))
#define Poke2(ADDR,VALUE) (*(volatile uint*)(uint)(ADDR) = (uint)(VALUE))

const uint Pia0PortA = 0xFF00;
const uint Pia0PortB = 0xFF02;

const uint IRQVEC = 0x010C;

const byte JMP_Extended = 0x7E;

struct prelude {
};

void memcpy(byte *dest, byte *src, uint count);

void Fatal(const char* s, uint arg);

#endif // _PRELUDE_H_
#line 1 "breakkey.h"
struct breakkey {
};  // Instance is named Break.

// Break_Handler is called on interrupt.
void Breakkey_Handler(void);
#line 1 "irq.h"
struct irq {
};

void Irq_Handler();
void Irq_Handler_RTI();
#line 1 "keyboard.h"
struct keyboard {
    byte matrix[2][8];
    byte current_matrix;
};  // Instance is named Keyboard.

void Keyboard_Handler(void);
#line 1 "prelude.h"
#ifndef _PRELUDE_H_
#define _PRELUDE_H_

typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned int word;
typedef unsigned int size_t;
typedef void (*func)(void);

#define true ((bool)1)
#define false ((bool)0)

#define Peek1(ADDR) (*(volatile byte*)(uint)(ADDR))
#define Poke1(ADDR,VALUE) (*(volatile byte*)(uint)(ADDR) = (byte)(VALUE))

#define Peek2(ADDR) (*(volatile uint*)(uint)(ADDR))
#define Poke2(ADDR,VALUE) (*(volatile uint*)(uint)(ADDR) = (uint)(VALUE))

const uint Pia0PortA = 0xFF00;
const uint Pia0PortB = 0xFF02;

const uint IRQVEC = 0x010C;

const byte JMP_Extended = 0x7E;

struct prelude {
};

void memcpy(byte *dest, byte *src, uint count);

void Fatal(const char* s, uint arg);

#endif // _PRELUDE_H_
#line 1 "real.h"
// Real time 

// This clock should steadily increase with each 60hz interrupt,
// except it rolls back to zero every 18.204444 hours.

// If you want to do something once per second, poll the seconds
// here, and see when it changes.  (You'll have to remember the
// previous value.)

struct real {
    byte ticks;  // Changes at 60Hz:  0 to 5
    byte decis;  // Tenths of a second: 0 to 9
    uint seconds;  // 0 to 65535
};  // Instance is named Real.

void Real_IncrementTicks(void);
#line 1 "vdg.h"
#if 0
void Pia1bOn(byte x) { *(volatile byte*)0xFF22 |= x; }
void Pia1bOff(byte x) { *(volatile byte*)0xFF22 &= ~x; }
void SetOrangeScreen() { Pia1bOn(0x08); }
void SetGreenScreen() { Pia1bOff(0x08); }
#endif

struct vdg {
};
#line 1 "w5100s_defs.h"
// github.com/strickyak/frobio/frob3/wiz/w5100s_defs.h
// copied from github.com/n6il/cocoio-dw/tests/w5100s.h
// Just the `#define`s.

#ifndef W5100S_DEFS_H_
#define W5100S_DEFS_H_

// #include <stdint.h>

/*********************************************************************
;* Title: W5100SEQU.asm
;*********************************************************************
;* Author: R. Allen Murphey
;*
;* License: Contributed 2021 by R. Allen Murphey to CoCoIO Development
;*
;* Description: WIZnet W5100S driver equates
;*
;* Documentation: https://www.wiznet.io/product-item/w5100s/
;*
;* Include Files: none
;*
;* Assembler: lwasm 1.4.2
;*
;* Revision History:
;* Rev #     Date      Who     Comments
;* -----  -----------  ------  ---------------------------------------
;* 01     2022/05/23   MRF     Convert from ASM to C
;* 00     2021         RAM     Initial equates from W5100S Data Sheet
;*********************************************************************/

/*
                              ; W5100S REGISTER / MEMORY MAP
                              ; 0x0000 - 0x002F Common Registers W5100 compatible
                              ; 0x0030 - 0x0088 Common Registers New W5100S
                              ; 0x0089 - 0x03FF -- Reserved --
                              ; 0x0400 - 0x04FF Socket 0 Registers
                              ; 0x0500 - 0x05FF Socket 1 Registers
                              ; 0x0600 - 0x06FF Socket 2 Registers
                              ; 0x0700 - 0x07FF Socket 3 Registers
                              ; 0x0800 - 0x3FFF -- Reserved --
                              ; 0x4000 - 0x5FFF TX Memory - default 2KB per socket
                              ; 0x6000 - 0x7FFF RX Memory - default 2KB per socket
*/

                                /* COMMON REGISTERS 0x0000-0x0088 */
#define MR         0x0000       /* Mode Register */
#define MR_IND     0x01         /* MR: Indirect */
#define MR_AINC    0x02         /* MR: Auto-Incrememt */
#define MR_RST     0x80         /* MR: Reset */ 
#define GAR0       0x0001       /* Gateway Address Register 0 */
#define GAR1       0x0002       /* Gateway Address Register 1 */
#define GAR2       0x0003       /* Gateway Address Register 2 */
#define GAR3       0x0004       /* Gateway Address Register 3 */
#define SUBR0      0x0005       /* Subnet Mask Address Register 0 */
#define SUBR1      0x0006       /* Subnet Mask Address Register 1 */
#define SUBR2      0x0007       /* Subnet Mask Address Register 2 */
#define SUBR3      0x0008       /* Subnet Mask Address Register 3 */
#define SHAR0      0x0009       /* Source Hardware Address Register 0 */
#define SHAR1      0x000A       /* Source Hardware Address Register 1 */
#define SHAR2      0x000B       /* Source Hardware Address Register 2 */
#define SHAR3      0x000C       /* Source Hardware Address Register 3 */
#define SHAR4      0x000D       /* Source Hardware Address Register 4 */
#define SHAR5      0x000E       /* Source Hardware Address Register 5 */
#define SIPR0      0x000F       /* Source IP Address Register 0 */
#define SIPR1      0x0010       /* Source IP Address Register 1 */
#define SIPR2      0x0011       /* Source IP Address Register 2 */
#define SIPR3      0x0012       /* Source IP Address Register 3 */
#define INTPTMR0   0x0013       /* Interrupt Pending Time Register 0 */
#define INTPTMR1   0x0014       /* Interrupt Pending Time Register 1 */
#define IR         0x0015       /* Interrupt Register */
#define IMR        0x0016       /* Interrupt Mask Register */
#define RTR0       0x0017       /* Retransmission Time Register 0 */
#define RTR1       0x0018       /* Retransmission Time Register 1 */
#define RCR        0x0019       /* Retransmission Count Register */
#define RMSR       0x001A       /* RX Memory Size Register */
#define TMSR       0x001B       /* TX Memory Size Register */
                                /* 0x001C-0x001F -- Reserved -- */
#define IR2        0x0020       /* Interrupt2 Register */
#define IMR2       0x0021       /* Interrupt2 Mask Register */
                                /* 0x0022-0x0027 -- Reserved -- */
#define PTIMER     0x0028       /* PPP LCP Request Timer */
#define PMAGIC     0x0029       /* PPP LCP Magic Number */
#define UIPR0      0x002A       /* Unreachable IP Address Register 0 */
#define UIPR1      0x002B       /* Unreachable IP Address Register 1 */
#define UIPR2      0x002C       /* Unreachable IP Address Register 2 */
#define UIPR3      0x002D       /* Unreachable IP Address Register 0 */
#define UPORTR0    0x002E       /* Unreachable Port Register 0 */
#define UPORTR1    0x002F       /* Unreachable Port Register 1 */
#define MR2        0x0030       /* Mode2 Register */
                                /* 0x0031 --  Reserved -- */
#define PHAR0      0x0032       /* Destination Hardware Address on PPPoE Register 0 */
#define PHAR1      0x0033       /* Destination Hardware Address on PPPoE Register 1 */
#define PHAR2      0x0034       /* Destination Hardware Address on PPPoE Register 2 */
#define PHAR3      0x0035       /* Destination Hardware Address on PPPoE Register 3 */
#define PHAR4      0x0036       /* Destination Hardware Address on PPPoE Register 4 */
#define PHAR5      0x0037       /* Destination Hardware Address on PPPoE Register 5 */
#define PSIDR0     0x0038       /* Session ID on PPPoE Register 0 */
#define PSIDR1     0x0039       /* Session ID on PPPoE Register 1 */
#define PMRUR0     0x003A       /* Maximum Receive Unit on PPPoE Register 0 */
#define PMRUR1     0x003B       /* Maximum Receive Unit on PPPoE Register 1 */
#define PHYSR0     0x003C       /* PHY Status Register 0 */
#define PHYSR1     0x003D       /* PHY Status Register 1 */
#define PHYAR      0x003E       /* PHY Address Value */
#define PHYRAR     0x003F       /* PHY Register Address */
#define PHYDIR0    0x0040       /* PHY Data Input Register 0 */
#define PHYDIR1    0x0041       /* PHY Data Input Register 1 */
#define PHYDOR0    0x0042       /* PHY Data Output Register 0 */
#define PHYDOR1    0x0043       /* PHY Data Output Register 1 */
#define PHYACR     0x0044       /* PHY Access Register */
#define PHYDIVR    0x0045       /* PHY Division Register */
#define PHYCR0     0x0046       /* PHY Control Register 0 */
#define PHYCR1     0x0047       /* PHY Control Register 1 */
                                /* 0x0048-0x004B -- Reserved -- */
#define SLCR       0x004C       /* SOCKET-less Command Register */
#define SLCR_P     0x01         /* Ping */
#define SLCR_A     0x02         /* ARP */
#define SLRTR0     0x004D       /* SOCKET-less Retransmission Time Register 0 */
#define SLRTR1     0x004E       /* SOCKET-less Retransmission Time Register 1 */
#define SLRCR      0x004F       /* SOCKET-less Retransmission Count Register */
#define SLPIPR0    0x0050       /* SOCKET-less Peer IP Address Register 0 */
#define SLPIPR1    0x0051       /* SOCKET-less Peer IP Address Register 1 */
#define SLPIPR2    0x0052       /* SOCKET-less Peer IP Address Register 2 */
#define SLPIPR3    0x0053       /* SOCKET-less Peer IP Address Register 3 */
#define SLPHAR0    0x0054       /* SOCKET-less Peer Hardware Address Register 0 */
#define SLPHAR1    0x0055       /* SOCKET-less Peer Hardware Address Register 1 */
#define SLPHAR2    0x0056       /* SOCKET-less Peer Hardware Address Register 2 */
#define SLPHAR3    0x0057       /* SOCKET-less Peer Hardware Address Register 3 */
#define SLPHAR4    0x0058       /* SOCKET-less Peer Hardware Address Register 4 */
#define SLPHAR5    0x0059       /* SOCKET-less Peer Hardware Address Register 5 */
#define PINGSEQR0  0x005A       /* PING Sequence Number Register 0 */
#define PINGSEQR1  0x005B       /* PING Sequence Number Register 1 */
#define PINGIDR0   0x005C       /* PING ID Register 0 */
#define PINGIDR1   0x005D       /* PING ID Register 1 */
#define SLIMR      0x005E       /* SOCKET-less Interrupt Mask Register */
#define SLIMR_P    0x01         /* Ping */
#define SLIMR_A    0x02         /* ARP */
#define SLIMR_T    0x04         /* Timeout */
#define SLIR       0x005F       /* SOCKET-less Interrupt Register */
#define SLIR_P     0x01         /* Ping */
#define SLIR_A     0x02         /* ARP */
#define SLIR_T     0x04         /* Timeout */
                                /* 0x0060-0x006A -- Reserved -- */
#define CLKLCKR    0x0070       /* Clock Lock Register */
#define NETLCKR    0x0071       /* Network Lock Register */
#define PHYLCKR    0x0072       /* PHY Lock Register */
                                /* 0x0073-0x007F -- Reserved -- */
#define VERR       0x0080       /* Chip Version */
                                /* 0x0081 -- Reserved -- */
#define TCNTR0     0x0082       /* 100us Ticker Counter Register 0 */
#define TCNTR1     0x0083       /* 100us Ticker Counter Register 1 */
                                /* 0x0084-0x0087 -- Reserved -- */
#define TCNTCLR    0x0088       /* 100us Ticker Counter Clear Register */

                    /* SOCKET 0 REGISTERS 0x0400-0x04FF 45 bytes per socket */
                    /* SOCKET 1 REGISTERS 0x0500-0x05FF 45 bytes per socket */
                    /* SOCKET 2 REGISTERS 0x0600-0x06FF 45 bytes per socket */
                    /* SOCKET 3 REGISTERS 0x0700-0x07FF 45 bytes per socket */

/* Socket Register Macros */

#define SKREGBASE   0x0400      /* Base Address for Sock Registers (Sock 0) */
#define SKREGBLK   0x0100      /* Size of Sock Register Blocks */
#define SKREGBLKOFS(k) (SKREGBLK*k) /* Socket k Register Block Offset */
#define SKREGADR(k) (SKREGBASE+SKREGBLKOFS(k)) /* Sock k Reg Blk Address */
#define SKREGABS(k, r) (SKREGADR(k)+r) /* Sock k Register r Address Absolute */
#define SKREGREL(o, r) (SKREGBASE+o+r) /* Sock k Register r Address Relative */
#define SKPTRREG(p, r) (p->skrgaddr + r)

/* Socket Buffer Macros */
#define SKTXBUFBASE  0x4000
#define SKRXBUFBASE  0x6000
#define SKBUFSIZE    0x0800
#define SKBUFMASK    0x07FF
#define SKRXBSTRT(k)    (SKRXBUFBASE+(k*SKBUFSIZE))
#define SKTXBSTRT(k)    (SKTXBUFBASE+(k*SKBUFSIZE))
#define SKRXBEND(k)    (SKRXBUFBASE+((k+1)*SKBUFSIZE)-1)
#define SKTXBEND(k)    (SKTXBUFBASE+((k+1)*SKBUFSIZE)-1)

/* Socket Register Defs */

#define SK_MR      0x0000       /* Socket K Mode Register */
#define SK_MR_PCLR 0xF0         /* Protocol: Clear */
#define SK_MR_PMSK 0x0F         /* Protocol: Mask */
#define SK_MR_CLSD 0x00         /* Protocool: Socket Closed */
#define SK_MR_TCP  0x01         /* Protocol: TCP */
#define SK_MR_UDP  0x02         /* Protocol: UDP */
#define SK_MR_IPRW 0x03         /* Protocol: IP Raw */
#define SK_MR_MACR 0x04         /* Protocol: MAC Raw */
#define SK_MR_ND   0x20         /* Protocol: TCP No Delay*/
#define SK_MR_CLND 0xDF         /* Protocol: Clear TCP No Delay */
#define SK_CR      0x0001       /* Socket K Command Register */
#define SK_CR_OPEN 0x01
#define SK_CR_LSTN 0x02
#define SK_CR_CONN 0x04
#define SK_CR_DISC 0x08
#define SK_CR_CLOS 0x10
#define SK_CR_SEND 0x20
#define SK_CR_KEEP 0x22
#define SK_CR_RECV 0x40
#define SK_IR      0x0002       /* Socket K Interrupt Register */
#define SK_IR_CON  0x01
#define SK_IR_DISC 0x02
#define SK_IR_RECV 0x04
#define SK_IR_TOUT 0x08
#define SK_IR_TXOK 0x10
#define SK_SR      0x0003       /* Socket K Status Register */
#define SK_SR_CLOS 0x00
#define SK_SR_INIT 0x13
#define SK_SR_LSTN 0x14
#define SK_SR_SYNS 0x15
#define SK_SR_SYNR 0x16
#define SK_SR_ESTB 0x17
#define SK_SR_FINW 0x18
#define SK_SR_TIMW 0x1B
#define SK_SR_CLWT 0x1C
#define SK_SR_LACK 0x1D
#define SK_SR_UDP  0x22
#define SK_SR_IPRW 0x32
#define SK_SR_MACR 0x42
#define SK_PORTR0  0x0004       /* Socket K Source Port Register 0 */
#define SK_PORTR1  0x0005       /* Socket K Source Port Register 1 */
#define SK_DHAR0   0x0006       /* Socket K Destination Hardware Address Register 0 */
#define SK_DHAR1   0x0007       /* Socket K Destination Hardware Address Register 1 */
#define SK_DHAR2   0x0008       /* Socket K Destination Hardware Address Register 2 */
#define SK_DHAR3   0x0009       /* Socket K Destination Hardware Address Register 3 */
#define SK_DHAR4   0x000A       /* Socket K Destination Hardware Address Register 4 */
#define SK_DHAR5   0x000B       /* Socket K Destination Hardware Address Register 5 */
#define SK_DIPR0   0x000C       /* Socket K Destination IP Address Register 0 */
#define SK_DIPR1   0x000D       /* Socket K Destination IP Address Register 1 */
#define SK_DIPR2   0x000E       /* Socket K Destination IP Address Register 2 */
#define SK_DIPR3   0x000F       /* Socket K Destination IP Address Register 3 */
#define SK_DPORTR0 0x0010       /* Socket K Destination Port Register 0 */
#define SK_DPORTR1 0x0011       /* Socket K Destination Port Register 1 */
#define SK_MSS0    0x0012       /* Socket K Maximum Segment Size Register 0 */
#define SK_MSS1    0x0013       /* Socket K Maximum Segment Size Register 1 */
#define SK_PROTOR  0x0014       /* Socket K IP Protocol Register */
#define SK_TOS     0x0015       /* Socket K IP Type Of Service */
#define SK_TTL     0x0016       /* Socket K IP Time To Live */
                                /* 0x0X17-0x0X1D -- Reserved -- */
#define SK_RXBUF_SIZE  $041E    /* Socket K RX Buffer Size */
#define SK_TXBUF_SIZE  $041F    /* Socket K TX Buffer Size */
#define SK_TX_FSR0 0x0020       /* Socket K TX Free Size Register 0 */
#define SK_TX_FSR1 0x0021       /* Socket K TX Free Size Register 1 */
#define SK_TX_RD0  0x0022       /* Socket K TX Read Pointer 0 */
#define SK_TX_RD1  0x0023       /* Socket K TX Read Pointer 1 */
#define SK_TX_WR0  0x0024       /* Socket K TX Write Pointer 0 */
#define SK_TX_WR1  0x0025       /* Socket K TX Write Pointer 1 */
#define SK_RX_RSR0 0x0026       /* Socket K RX Received Size Register 0 */
#define SK_RX_RSR1 0x0027       /* Socket K RX Received Size Register 1 */
#define SK_RX_RD0  0x0028       /* Socket K RX Read Pointer 0 */
#define SK_RX_RD1  0x0029       /* Socket K RX Read Pointer 1 */
#define SK_RX_WR0  0x002A       /* Socket K RX Write Pointer 0 */
#define SK_RX_WR1  0x002B       /* Socket K RX Write Pointer 1 */
#define SK_IMR     0x002C       /* Socket K Interrupt Mask Register */
#define SK_FRAGR0  0x002D       /* Socket K Fragment Offset in IP Header Register 0 */
#define SK_FRAGR1  0x002E       /* Socket K Fragment Offset in IP Header Register 1 */
#define SK_MR2     0x002F       /* Socket K Mode 2 Register */
#define SK_KPALVTR 0x0030       /* Socket K Keep-alive Timer Register */
                                /* 0x0X31 -- Reserved -- */
#define SK_RTR0    0x0032       /* Socket K Retransmission Time Register 0 */
#define SK_RTR1    0x0033       /* Socket K Retransmission Time Register 1 */
#define SK_RCR     0x0034       /* Socket K Retransmission Count Register */

                                /* End of Equates */

#endif // W5100S_DEFS_H_
#line 1 "wall.h"
// Wall Time 

// Wall Time might be adjusted, even backwards.
// Different cocos may use different time zones.
// This is meant for displaying clocks and weather and the like.

struct wall {
    byte second; // 0 to 59
    byte minute; // 0 to 59
    byte hour;  // 0 to 23

    byte day;   // 1 to 31
    byte month; // 1 to 12
    byte year2000;  // e.g. 25 means 2025
    char dow[3];  // e.g. Mon
    char moy[3];  // e.g. Jan

    // If hour rolls over,
    // these values are copied to day, month, etc.
    byte next_day;
    byte next_month;
    byte next_year2000;
    char next_dow[3];
    char next_moy[3];
}; // Instance is named Wall.

void Wall_IncrementSecond(void);
#line 1 "wiznet.h"
struct wiznet {
    struct wiz_port *wiz_port;
};

void Wiznet_Handler(void);
void Network_Handler(void);

void Wiznet_Init(void);
void Network_Init(void);


#line 1 "SYNTHETIC"
struct breakkey Breakkey;
struct irq Irq;
struct keyboard Keyboard;
struct prelude Prelude;
struct real Real;
struct vdg Vdg;
struct wall Wall;
struct wiznet Wiznet;
#line 1 "breakkey.c"
void DoBreak(void) {
    // TODO
}

// Break_Handler is called on interrupt.
void Breakkey_Handler(void) {
    const byte probe = ~(byte)0x04;
    Poke1(Pia0PortB, probe);  // Place the keyboard probe signal for BREAK
    byte sense = Peek1(Pia0PortA);

    if ((sense & 0x40) == 0) {  // Check the sense bit for BREAK
        DoBreak();
    }
}
#line 1 "irq.c"
void Network_Handler(void);

func irq_schedule[6] = {
    Network_Handler,
    Breakkey_Handler,
    Network_Handler,
    Breakkey_Handler,
    Network_Handler,
    Breakkey_Handler,
};

void Irq_Handler() {
    Real_IncrementTicks();
    irq_schedule[Real.ticks]();
}

void Irq_Handler_Wrapper() {
    asm volatile(
        "\n_Irq_Handler_RTI:\n"
        "  JSR _Irq_Handler\n"
        "  RTI\n");
    Poke2(0, Irq_Handler);
}
#line 1 "keyboard.c"
static void Scan(byte* p) {
    const uint out_port = Pia0PortB;
    const uint in_port = Pia0PortA;

    Poke1(out_port, ~(byte)0x01);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x02);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x04);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x08);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x10);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x20);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, ~(byte)0x40);
    *p++ = 0x7F & ~Peek1(in_port);

    Poke1(out_port, 0x7F);
    *p++ = 0x7F & ~Peek1(in_port);
}

// Did the keyboard change from the previous to the current matrix?
static bool Changed(byte* current, byte* prev) {
    // Convert to uint* to use a stride of 2.
    uint* a = (uint*)current;
    uint* b = (uint*)prev;
    // Now four times with a stride of 2, for 8 bytes.
    if (*a++ != *b++) return true;
    if (*a++ != *b++) return true;
    if (*a++ != *b++) return true;
    if (*a++ != *b++) return true;
    return false; // No change.
}

static void SendKeyboardPacket(byte* p) {
}

void Keyboard_Handler() {
    byte current = Keyboard.current_matrix;
    byte other = !current;

    Scan(Keyboard.matrix[current]);
    bool changed = Changed(Keyboard.matrix[current], Keyboard.matrix[other]);
    if (changed) SendKeyboardPacket(Keyboard.matrix[current]);

    Keyboard.current_matrix = other;
}
#line 1 "main.c"
extern void Spacewar_Main(void);

void main() {
    Poke1(IRQVEC, JMP_Extended);
    Poke2(IRQVEC+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC+1, Irq_Handler_RTI);

    Vdg_Init();
    Network_Init();
    Spacewar_Main();
}


#if 0

5117 BFF2 0100             (          bas.asm):04499         LBFF2   FDB     SW3VEC  SWI3
5118 BFF4 0103             (          bas.asm):04500         LBFF4   FDB     SW2VEC  SWI2
5119 BFF6 010F             (          bas.asm):04501         LBFF6   FDB     FRQVEC  FIRQ
5120 BFF8 010C             (          bas.asm):04502         LBFF8   FDB     IRQVEC  IRQ
5121 BFFA 0106             (          bas.asm):04503         LBFFA   FDB     SWIVEC  SWI
5122 BFFC 0109             (          bas.asm):04504         LBFFC   FDB     NMIVEC  NMI
5123 BFFE A027             (          bas.asm):04505         LBFFE   FDB     RESVEC  RESET

#endif
#line 1 "prelude.c"
void memcpy(byte *dest, byte *src, uint count) {
    // If count is odd, copy an initial byte.
    if (count & 1) {
        *dest++ = *src++;
    }
    // Now use a stride of 2.
    count >>= 1;  // Divide count by 2.
    uint* d = (uint*)dest;
    uint* s = (uint*)src;
    for (uint i = 0; i < count; i++) {
        *d++ = *s++;
    }
}
#line 1 "real.c"
static void IncrementDecis() {
    if (Real.decis < 9) {
        ++Real.decis;
    } else {
        Real.decis = 0;
        ++Real.seconds;

        // Now go increment the Wall Time.
        Wall_IncrementSecond();
    }
}

void Real_IncrementTicks() {
    if (Real.ticks < 5) {
        ++Real.ticks;
    } else {
        Real.ticks = 0;
        IncrementDecis();
    }
}
#line 1 "vdg.c"
void TextAt0200() {
    // Video Dispaly Mode: Text (512)
    Poke1(0xFFC0, 0);
    Poke1(0xFFC2, 0);
    Poke1(0xFFC4, 0);
    // Frame buffer at $0200
    Poke1(0xFFC7, 0); // 1
    Poke1(0xFFC8, 0);
    Poke1(0xFFCA, 0);
    Poke1(0xFFCC, 0);
    Poke1(0xFFCE, 0);
    Poke1(0xFFD0, 0);
    Poke1(0xFFD2, 0);
    // Slow CPU
    Poke1(0xFFD6, 0);
    Poke1(0xFFD8, 0);

    // PUT THE VDG INTO ALPHA-GRAPHICS MODE
    Poke1(0xFF22, 0x07);
}

void Vdg_TextMode(uint addr, byte css) {
    TextAt0200();
}

void Vdg_Init() {
    TextAt0200();
}
#line 1 "wall.c"
static void IncrementDay() {
    // Rather than do Gregorian Date calculations,
    // we let the server tell us what the next day is.
    Wall.day = Wall.next_day;  
    Wall.month = Wall.next_month;  
    Wall.year2000 = Wall.next_year2000;  
    memcpy(Wall.dow, Wall.next_dow, 3);
    memcpy(Wall.moy, Wall.next_moy, 3);
}

static void IncrementHour() {
    if (Wall.hour < 23) {
        ++Wall.hour;
    } else {
        Wall.hour = 0;
        IncrementDay();
    }
}

static void IncrementMinute() {
    if (Wall.minute < 59) {
        ++Wall.minute;
    } else {
        Wall.minute = 0;
        IncrementHour();
    }
}

void Wall_IncrementSecond() {
    if (Wall.second < 59) {
        ++Wall.second;
    } else {
        Wall.second = 0;
        IncrementMinute();
    }
}
#line 1 "wiznet.c"
void Wiznet_Handler() {
}
void Network_Handler() {
    Wiznet_Handler();
}

void Wiznet_Init() {
}
void Network_Init() {
    Wiznet_Init();
}

/////////////////////////////////////////

// How to talk to the four hardware ports.
struct wiz_port {
  volatile byte command;
  volatile uint addr;
  volatile byte data;
};

// Axiom uses Socket 1 (of 0 thru 3).
uint const B = 0x500u;   // Socket Base
uint const T = 0x4800u;  // Transmit ring
uint const R = 0x6800u;  // Receive ring
#define RING_SIZE 2048
#define RING_MASK (RING_SIZE - 1)
typedef word tx_ptr_t;
typedef byte errnum;
#define WIZ (Wiznet.wiz_port)
#define OKAY ((errnum)0)
#define NOTYET ((errnum)1)

//////////////////////////////////////

byte WizGet1(word reg) {
  WIZ->addr = reg;
  return WIZ->data;
}
word WizGet2(word reg) {
  WIZ->addr = reg;
  byte z_hi = WIZ->data;
  byte z_lo = WIZ->data;
  return ((word)(z_hi) << 8) + (word)z_lo;
}
void WizGetN(word reg, void* buffer, word size) {
  volatile struct wiz_port* wiz = WIZ;
  byte* to = (byte*)buffer;
  wiz->addr = reg;
  for (word i = size; i; i--) {
    *to++ = wiz->data;
  }
}
void WizPut1(word reg, byte value) {
  WIZ->addr = reg;
  WIZ->data = value;
}
void WizPut2(word reg, word value) {
  WIZ->addr = reg;
  WIZ->data = (byte)(value >> 8);
  WIZ->data = (byte)(value);
}
void WizPutN(word reg, const void* data, word size) {
  volatile struct wiz_port* wiz = WIZ;
  const byte* from = (const byte*)data;
  wiz->addr = reg;
  for (word i = size; i; i--) {
    wiz->data = *from++;
  }
}

word WizTicks() { return WizGet2(0x0082 /*TCNTR Tick Counter*/); }
byte WizTocks() { return WizGet1(0x0082 /*TCNTR Tick Counter*/); }

bool ValidateWizPort(struct wiz_port* p) {
  byte status = p->command;
  return (status == 3);
}

void FindWizPort() {
    struct wiz_port* p = (void*)0xFF68;
    if (ValidateWizPort(p)) {
        Wiznet.wiz_port = p;
        return;
    }
    p = (void*)0xFF78;
    if (ValidateWizPort(p)) {
        Wiznet.wiz_port = p;
        return;
    }
    Fatal("NOWIZ", 0);
}

//////////////////////////////////////////

void WizIssueCommand(byte cmd) {
  WizPut1(B + SK_CR, cmd);
  while (WizGet1(B + SK_CR)) {
  }
}

void WizWaitStatus(byte want) {
  byte status;
  byte stuck = 200;
  do {
    status = WizGet1(B + SK_SR);
    if (!--stuck) Fatal("W", status);
  } while (status != want);
}


////////////////////////////////////////
////////////////////////////////////////

// returns tx_ptr
tx_ptr_t WizReserveToSend( size_t n) {
  // PrintH("ResTS %x;", n);
  // Wait until free space is available.
  word free_size;
  do {
    free_size = WizGet2(B + SK_TX_FSR0);
    // PrintH("Res free %x;", free_size);
  } while (free_size < n);

  return WizGet2(B + SK_TX_WR0) & RING_MASK;
}

tx_ptr_t WizDataToSend( tx_ptr_t tx_ptr,
                       const char* data, size_t n) {
  word begin = tx_ptr;   // begin: Beneath RING_SIZE.
  word end = begin + n;  // end:  Sum may not be beneath RING_SIZE.

  if (end >= RING_SIZE) {
    word first_n = RING_SIZE - begin;
    word second_n = n - first_n;
    WizPutN(T + begin, data, first_n);
    WizPutN(T, data + first_n, second_n);
  } else {
    WizPutN(T + begin, data, n);
  }
  return (n + tx_ptr) & RING_MASK;
}
tx_ptr_t WizBytesToSend( tx_ptr_t tx_ptr,
                        const byte* data, size_t n) {
  return WizDataToSend(tx_ptr, (char*)data, n);
}

void WizFinalizeSend( size_t n) {
  word tx_wr = WizGet2(B + SK_TX_WR0);
  tx_wr += n;
  WizPut2(B + SK_TX_WR0, tx_wr);
  WizIssueCommand(SK_CR_SEND);
}

errnum WizCheck() {
  byte ir = WizGet1(B + SK_IR);  // Socket Interrupt Register.
  if (ir & SK_IR_TOUT) {         // Timeout?
    return SK_IR_TOUT;
  }
  if (ir & SK_IR_DISC) {  // Disconnect?
    return SK_IR_DISC;
  }
  return OKAY;
}
errnum WizSendChunk( char* data, size_t n) {
  // PrintH("Ax WizSendChunk %x@%x : %x %x %x %x %x", n, data, data[0], data[1],
  // data[2], data[3], data[4]);
  errnum e = WizCheck();
  if (e) return e;
  tx_ptr_t tx_ptr = WizReserveToSend(n);
  WizDataToSend(tx_ptr, data, n);
  WizFinalizeSend(n);
  // PrintH("Ax WSC.");
  return OKAY;
}

//////////////////////////////////////////////////////////////////

errnum WizRecvGetBytesWaiting(word* bytes_waiting_out) {
  errnum e = WizCheck();
  if (e) return e;

  *bytes_waiting_out = WizGet2(B + SK_RX_RSR0);  // Unread Received Size.
  return OKAY;
}

errnum WizRecvChunkTry( char* buf, size_t n) {
  word bytes_waiting = 0;
  errnum e = WizRecvGetBytesWaiting(& bytes_waiting);
  if (e) return e;
  if (bytes_waiting < n) return NOTYET;

  word rd = WizGet2(B + SK_RX_RD0);
  word begin = rd & RING_MASK;  // begin: Beneath RING_SIZE.
  word end = begin + n;         // end: Sum may not be beneath RING_SIZE.

  if (end >= RING_SIZE) {
    word first_n = RING_SIZE - begin;
    word second_n = n - first_n;
    WizGetN(R + begin, buf, first_n);
    WizGetN(R, buf + first_n, second_n);
  } else {
    WizGetN(R + begin, buf, n);
  }

  WizPut2(B + SK_RX_RD0, rd + n);
  WizIssueCommand(SK_CR_RECV);  // Inform socket of changed SK_RX_RD.
  return OKAY;
}

errnum WizRecvChunk( char* buf, size_t n) {
  // PrintH("WizRecvChunk %x...", n);
  errnum e;
  do {
    e = WizRecvChunkTry(buf, n);
  } while (e == NOTYET);
  // PrintH("WRC %x: %x %x %x %x %x.", n, buf[0], buf[1], buf[2], buf[3],
  // buf[4]);
  return e;
}
errnum WizRecvChunkBytes( byte* buf, size_t n) {
  return WizRecvChunk((char*)buf, n);
}

////////////////////////////////////////
////////////////////////////////////////
#line 1 "../games/spacewar/spacewar.c"



void Spacewar_Main() {
    Vdg_TextMode(0x200, 0);

    asm volatile("  andcc #^$50"); // Allow interrupts

    byte* p = 0x300;
    while (true) {
        p[0]++;
        if (!p[0]) {
            p[2]++;
            if (!p[2]) {
                p[4]++;
            }
        }
    }
}
