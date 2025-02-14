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
    // Clear the VSYNC IRQ by reading PortB output register.
    uint const clear_irq = Pia0PortB;
    (void) Peek1(clear_irq);

    Real_IncrementTicks();
    irq_schedule[Real.ticks]();

    SpinIrq();
}

void Irq_Handler_Wrapper() {
    asm volatile(
        "\n_Irq_Handler_RTI:\n"
        "  JSR _Irq_Handler\n"
        "  RTI\n");
    Poke2(0, Irq_Handler);
}
