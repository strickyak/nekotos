#include "nekot1/private.h"

// main.c

// Use our alternate data sections.
gword _More0 gZEROED; // not .bss
gword _More1 gZEROED = 0x9998; // not .data
gword _Final __attribute__ ((section (".final"))) = 0x9990;
gword _Final_Startup __attribute__ ((section (".final.startup"))) = 0x9991;

// pia_reset_sequence table traced from coco3 startup.
struct pia_reset_sequence {
    gword addr;
    gbyte value;
} pia_reset_sequence[] gSETUP_DATA = {
    { 0xff21, 0x00 },  // choose data direction
    { 0xff23, 0x00 },  // choose data direction
    { 0xff20, 0xfe },  // bit 0 input; rest are outputs.
    { 0xff22, 0xfa },  // bit 1 and bits 3-7 are outputs.
    { 0xff21, 0x34 },
    { 0xff23, 0x34 },
    { 0xff22, 0x00 },  // output all 0s on Pia1 PortB
    { 0xff20, 0x02 },
    { 0xff01, 0x00 },  // choose data direction
    { 0xff03, 0x00 },  // choose data direction
    { 0xff00, 0x00 },  // inputs
    { 0xff02, 0xff },  // outputs
    { 0xff01, 0x34 },
    { 0xff03, 0x34 },
    { 0 }
};

void ClearPage256(gword p) {
    for (gword i = 0; i<256; i+=2) {
        gPoke2(p+i, 0x0000);
    }
}

void entry_wrapper() {
    asm volatile("\n"
        "  .globl entry \n"
        "entry:         \n"
        "  orcc #$50    \n"  // No IRQs, FIRQs, for now.
        "  lds #$01FE   \n"  // Reset the stack
        "  jmp _main    \n"
        );
}

extern void embark(void);

gword PinDownGlobalNames[] gSETUP_DATA = {
    // Taking the address of `embark` prevents it from being
    // inlined inside main.  Only things inlined inside main
    // are in the area called `.text.startup` which gets
    // recycled after setup, when we embark.
    (gword) embark,

    // Taking the address of other things in the kernel
    // prevents them from being optimized away.
    // Things that might be referenced by games
    // need to exist in the compiled kernel!
    (gword) Breakkey_Handler,
    (gword) Irq_Handler,
    (gword) Irq_Handler_entry,
    (gword) Irq_Handler_Wrapper,
    (gword) Network_Handler,
    (gword) gAlloc64,
    (gword) gFree64,
    (gword) Reset64,
    (gword) gSendCast,
    (gword) gReceiveCast64,

    (gword) gTextScreen,
    (gword) gPMode1Screen,
    (gword) gModeScreen,
    (gword) xAfterSetup,
    (gword) xSendControlPacket,
    (gword) gNetworkLog,
    (gword) gFatal,
    (gword) PutStr,
    (gword) PutChar,

    (gword) entry_wrapper,
    (gword) &_More0,
    (gword) &_More1,
    (gword) &_Final,
    (gword) &_Final_Startup,
    (gword) &gScore,
    (gword) &gReal,
    (gword) &gWall,
    (gword) &gConfig,
};

void PlaceOpcodeJMP(gword at, gfunc to) {
    gPoke1(at+0, OPCODE_JMP_Extended);
    gPoke2(at+1, to);
}

// Interrupt Relays
gword coco2_relays[] gSETUP_DATA = {
    0x0100,
    0x0103,
    0x010F,
    0x010C,
    0x0106,
    0x0109,
};
gword coco3_relays[] gSETUP_DATA = {
    0xFFEE,
    0xFFF1,
    0xFFF4,
    0xFFF7,
    0xFFFA,
    0xFFFD,
};
gfunc handlers[] gSETUP_DATA = {
    gFatalSWI3,
    gFatalSWI2,
    gFatalFIRQ,
    Irq_Handler_entry,
    gFatalSWI1,
    gFatalNMI,
};

char StrNekotMicrokernel[] gSETUP_DATA = "\nNEKOT MICROKERNEL... ";
char StrReady[] gSETUP_DATA = " READY\n";

void setup(void) {
    ClearPage256(0x0000); // .bss
    ClearPage256(0x0200); // vdg console p1
    ClearPage256(0x0300); // vdg console p2
    ClearPage256(0x0400); // chunks of 64-gbyte

    // Coco3 in Compatibility Mode.
    gPoke1(0xFF90, 0x80);
    gPoke1(0xFF91, 0x00);

    Alloc64_Init();  // first 4 chunks in 0x04XX.
    Kern_Init();

    // Redirect the 6 Interrupt Relays to our handlers.
    for (gbyte i = 0; i < 6; i++) {
        PlaceOpcodeJMP(coco2_relays[i], handlers[i]);
        PlaceOpcodeJMP(coco3_relays[i], handlers[i]);
    }

    Console_Init();
    for (struct pia_reset_sequence *p = pia_reset_sequence; p->addr; p++) {
        gPoke1(p->addr, p->value);
    }
    Vdg_Init();

    PutStr(StrNekotMicrokernel);
    Spin_Init();
    Network_Init();
    HelloMCP();

    gPeek1(0xFF02);        // Clear VSYNC IRQ // TODO
    gPoke1(0xFF03, 0x35);  // +1: Enable VSYNC (FS) IRQ

    PutStr(StrReady);
}

void embark(void) {
    // Wipe out the setup/startup code, to prove it is never needed again.
    for (gword p = 2 + (gword)&_Final;
         p < (gword)_Final_Startup;
         p+=2) {
        gPoke2(p, 0x3F3F);
    }

    // gEnableIrq();  // Let StartTask do it.
    StartTask((gword)ChatTask); // Start the no-game task.
}

int main() {
    setup();
    embark();

    // NOT REACHED

    gPin(PinDownGlobalNames);
    gFatal("MAIN", 0);
}
