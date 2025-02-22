// main.c

word More0 MORE_DATA;
word More1 MORE_DATA = 0x9998;
word KernFinalCanary KERN_FINAL = 0x9990;
word StartupFinalCanary STARTUP_FINAL = 0x9991;

// This is a "Null Game" that does nothing.
void CWait(void) {
    ALLOW_IRQ();

    while (1) {
        // wait for any interrupt.
        asm volatile ("cwai #$00");
        SpinCWait();
    }
}

void FatalSpin(const char *why) {
    // Work around an infinite-loop bug by making it conditional
    // on something that will never be false.
    if (why) {
        while (1) {
            Cons[0]++;
        }
    }
}

void Fatal(const char* why, word arg) {
    INHIBIT_IRQ();

    Vdg_SetConsoleMode();
    Console_Printf("\nFATAL (%s, %x, %u, %d.)", why, arg, arg, arg);
    FatalSpin(why);
}

void Main_Main() {
    // Wipe out the startup code, to prove it is never needed again.
    for (vptr p = (vptr)&KernFinalCanary; p < (vptr)StartupFinalCanary; p++) {
        *p = 0;
    }

    ALLOW_IRQ();
    NoGameMain();
    // CWait();
    //XXX Spacewar_Main();
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
    (word) Irq_Handler_RTI,
    (word) Irq_Handler_Wrapper,
    (word) Network_Handler,
    // (word) Vdg_Init,

    // (word) Console_Init,
    (word) Vdg_GameText,
    (word) Vdg_GamePMode1,
    (word) Network_Log,
    // (word) Wiznet_Init,
    // (word) Network_Init,
    (word) CWait,
    (word) Fatal,
    (word) FatalSpin,
    (word) Console_Printf,
};

#if 0
void DeclareGlobls(void) {
  asm volatile("\n"
      "  .globl ClearPage0 \n"
      "  .globl Main_Main   \n"

      "  .globl Breakkey_Handler   \n"
      "  .globl Irq_Handler   \n"
      "  .globl Irq_Handler_RTI   \n"
      "  .globl Irq_Handler_Wrapper   \n"
      "  .globl Network_Handler   \n"
      "  .globl Vdg_Init   \n"

      "  .globl Console_Init   \n"
      "  .globl Vdg_GameText   \n"
      "  .globl Vdg_GamePMode1   \n"
      "  .globl Wiznet_Handler   \n"
      "  .globl Network_Handler   \n"
      "  .globl Network_Log   \n"
      "  .globl Wiznet_Init   \n"
      "  .globl Network_Init   \n"
      "  .globl CWait   \n"
      "  .globl Fatal   \n"
      "  .globl FatalSpin   \n"
      "  .globl Console_Printf   \n"
      );
}
#endif


int main() {
    Poke2(0, (word)PinDown);
    Poke2(0, More0);
    Poke2(0, More1);
    Poke2(0, KernFinalCanary);
    Poke2(0, StartupFinalCanary);

    asm volatile("\n"
        "  orcc #$50\n"   // No IRQs, FIRQs, for now.
        "  lds #$01FE\n"  // Reset the stack
        "  clra\n"
        "  clrb\n"
        "  tfr d,u\n"     // Initial NULL frame pointer.
        "  tfr b,dp\n"    // Direct page is zero page.
        );

    //? DeclareGlobls();
    ClearPage0();
    Kern_Init();
    // ================================
    // Set the IRQ vector code, for Coco 1 or 2.
    Poke1(IRQVEC_COCO12, JMP_Extended);
    Poke2(IRQVEC_COCO12+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO12+1, Irq_Handler_RTI);

    // Set the IRQ vector code, for Coco 3.
    Poke1(IRQVEC_COCO3, JMP_Extended);
    Poke2(IRQVEC_COCO3+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO3+1, Irq_Handler_RTI);

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
