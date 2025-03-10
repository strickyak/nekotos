#include "nekot1/private.h"

void Network_Handler(void);
void KeyboardHandler(void);

void NOOP() {}

func Irq_FocusGameSchedule[6] = {
    Network_Handler,
    Breakkey_Handler,

    Network_Handler,
    Breakkey_Handler,

    Network_Handler,
    Breakkey_Handler,
};

func Irq_PassiveGameSchedule[6] = {
    Network_Handler,
    Breakkey_Handler,

    Network_Handler,
    KeyboardHandler,

    Network_Handler,
    KeyboardHandler,
};

func Irq_FocusShellSchedule[6] = {
    KeyboardHandler,
    Breakkey_Handler,

    KeyboardHandler,
    Breakkey_Handler,

    KeyboardHandler,
    Breakkey_Handler,
};

void Irq_Handler() {
    Kern.in_irq = true;
    SpinIrq();

    // Clear the VSYNC IRQ by reading PortB output register.
    word const clear_irq = Pia0PortB;
    (void) Peek1(clear_irq);

    Real_IncrementTicks();
    Breakkey_Handler();

    assert(gReal.ticks < 6);
    if (Kern.focus_game) {
// Console_Printf("F");
        Irq_FocusGameSchedule[gReal.ticks]();
    } else if (Kern.in_game) {
        Irq_PassiveGameSchedule[gReal.ticks]();
    } else {
// Console_Printf("$");
        Irq_FocusShellSchedule[gReal.ticks]();
    }

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
