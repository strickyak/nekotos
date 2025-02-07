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
