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
    // Clear the VSYNC IRQ by reading PortB output register.
    uint const clear_irq = Pia0PortB;
    (void) Peek1(clear_irq);
    Kern.in_irq = true;

    Real_IncrementTicks();
    if (Kern.in_game) {
        Irq_InGameSchedule[Real.ticks]();
    } else {
        Irq_NoGameSchedule[Real.ticks]();
    }
    SpinIrq();

    Kern.in_irq = false;
}

void Irq_Handler_Wrapper() {
    asm volatile("\n"
        "_Irq_Handler_RTI:  \n"
        "  JSR _Irq_Handler  \n"
        "  RTI  \n");
    Poke2(0, Irq_Handler);
}
