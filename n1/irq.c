#include "n1/private.h"

void Network_Handler(void);

func Irq_InGameSchedule[6] = {
    Network_Handler,
    Breakkey_Handler,

    Network_Handler,
    Breakkey_Handler,

    Network_Handler,
    Breakkey_Handler,
};

func Irq_NoGameSchedule[6] = {
    Breakkey_Handler,
    Breakkey_Handler,
    Breakkey_Handler,
    Breakkey_Handler,
    Breakkey_Handler,
    Breakkey_Handler,
};

void Irq_Handler() {
    Kern.in_irq = true;

    // Clear the VSYNC IRQ by reading PortB output register.
    word const clear_irq = Pia0PortB;
    (void) Peek1(clear_irq);

    Breakkey_Handler();
    Real_IncrementTicks();

    assert(N1Real.ticks < 6);
    if (Kern.in_game) {
        Irq_InGameSchedule[N1Real.ticks]();
    } else {
        Irq_NoGameSchedule[N1Real.ticks]();
    }

    SpinIrq();
    Kern.in_irq = false;
}

void Irq_Handler_Wrapper() {
    asm volatile("\n"
        "  .globl _Irq_Handler_entry  \n"
        "_Irq_Handler_entry:  \n"
        "  JSR _Irq_Handler  \n"
        "  RTI  \n");
    Poke2(0, Irq_Handler);
}
