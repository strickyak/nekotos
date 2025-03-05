#include "n1/private.h"

// main.c

// Use our alternate data sections.
word _More0 MORE_DATA; // not .bss
word _More1 MORE_DATA = 0x9998; // not .data
word _Final __attribute__ ((section (".final"))) = 0x9990;
word _Final_Startup __attribute__ ((section (".final.startup"))) = 0x9991;

#if 0
// This is a "Null Game" that does nothing.
void CWait(void) {
    ALLOW_IRQ();

    while (1) {
        // wait for any interrupt.
        asm volatile ("cwai #$00");
        SpinCWait();
    }
}
#endif

// pia_reset table traced from coco3 startup.
struct pia_reset { word addr; byte value; } pia_reset[] STARTUP_DATA = {
     { 0xff21, 0x00 },
     { 0xff23, 0x00 },
     { 0xff20, 0xfe },
     { 0xff22, 0xf8 },
     { 0xff21, 0x34 },
     { 0xff23, 0x34 },
     { 0xff22, 0x00 },
     { 0xff20, 0x02 },
     { 0xff01, 0x00 },
     { 0xff03, 0x00 },
     { 0xff00, 0x00 },
     { 0xff02, 0xff },
     { 0xff01, 0x34 },
     { 0xff03, 0x34 },
     { 0 }
};

void after_main() {
    // Wipe out the startup code, to prove it is never needed again.
    for (byte* p = sizeof(word) + (byte*)&_Final;
         p < (byte*)_Final_Startup;
         p++) {
        *p = 0;
    }

    ALLOW_IRQ();
    StartTask((word)ChatTask); // Start the no-game task.
}

void ClearPage0() {
    for (word p = 0; p<256; p+=2) {
        Poke2(p, 0);
    }
}

word PinDown[] STARTUP_DATA = {
    (word) after_main,

    (word) Breakkey_Handler,
    (word) Irq_Handler,
    (word) Irq_Handler_entry,
    (word) Irq_Handler_Wrapper,
    (word) Network_Handler,

    (word) N1GameShowsTextScreen,
    (word) N1GameShowsPMode1Screen,
    (word) N1GameShowsOtherScreen,
    (word) Network_Log,
    (word) Fatal,
    (word) Console_Printf,
};

void before_main() {
    asm volatile("\n"
        "  .globl entry \n"
        "entry:         \n"
        "  orcc #$50    \n"  // No IRQs, FIRQs, for now.
        "  lds #$01FE   \n"  // Reset the stack
        "  jmp _main    \n"
        );
}

int main() {
    Poke2(0, before_main);
    Poke2(0, (word)PinDown);
    Poke2(0, _More0);
    Poke2(0, _More1);
    Poke2(0, _Final);
    Poke2(0, _Final_Startup);

    ClearPage0();
    Kern_Init();
    Poke1(0xFF90, 0x80);
    Poke1(0xFF91, 0x00);

    // Set the IRQ vector code, for Coco 1 or 2.
    Poke1(IRQVEC_COCO12, JMP_Extended);
    //-- Poke2(IRQVEC_COCO12+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO12+1, Irq_Handler_entry);

    // Set the IRQ vector code, for Coco 3.
    Poke1(IRQVEC_COCO3, JMP_Extended);
    //-- Poke2(IRQVEC_COCO3+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO3+1, Irq_Handler_entry);

    Console_Init();
    for (struct pia_reset *p = pia_reset; p->addr; p++) {
        Poke1(p->addr, p->value);
    }
    Vdg_Init();

    Poke2(0, PutStr);
    Poke2(0, PutChar);

    Console_Printf("NEKOT %s\n", "MICROKERNEL");
    Spin_Init();

    Peek1(0xFF02);        // Clear VSYNC IRQ
    Poke1(0xFF03, 0x35);  // +1: Enable VSYNC (FS) IRQ
    Peek1(0xFF02);        // Clear VSYNC IRQ

    Network_Init();
    HelloMCP();

    // ================================
    after_main();
    Fatal("EXIT", 0);
}
