#include "n1/private.h"

// main.c

// Use our alternate data sections.
word More0 MORE_DATA; // not .bss
word More1 MORE_DATA = 0x9998; // not .data
word KernFinalCanary KERN_FINAL = 0x9990;
word StartupFinalCanary STARTUP_FINAL = 0x9991;

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

void Main_Main() {
    // Wipe out the startup code, to prove it is never needed again.
    for (byte* p = sizeof(word) + (byte*)&KernFinalCanary;
         p < (byte*)StartupFinalCanary;
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

STARTUP_DATA  word PinDown[] = {
    (word) Main_Main,

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
    Poke2(0, More0);
    Poke2(0, More1);
    Poke2(0, KernFinalCanary);
    Poke2(0, StartupFinalCanary);

    ClearPage0();
    Kern_Init();

    // Set the IRQ vector code, for Coco 1 or 2.
    Poke1(IRQVEC_COCO12, JMP_Extended);
    //-- Poke2(IRQVEC_COCO12+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO12+1, Irq_Handler_entry);

    // Set the IRQ vector code, for Coco 3.
    Poke1(IRQVEC_COCO3, JMP_Extended);
    //-- Poke2(IRQVEC_COCO3+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO3+1, Irq_Handler_entry);

    Vdg_Init();
    Console_Init();

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
    Main_Main();
    Fatal("EXIT", 0);
}
