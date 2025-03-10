#include "nekot1/private.h"

void Network_Handler(void);
void KeyboardHandler(void);

void NOOP() {}

gfunc Irq_FocusGameSchedule[6] = {
    Network_Handler,
    Breakkey_Handler,

    Network_Handler,
    Breakkey_Handler,

    Network_Handler,
    Breakkey_Handler,
};

gfunc Irq_PassiveGameSchedule[6] = {
    Network_Handler,
    Breakkey_Handler,

    Network_Handler,
    KeyboardHandler,

    Network_Handler,
    KeyboardHandler,
};

gfunc Irq_FocusShellSchedule[6] = {
    KeyboardHandler,
    Breakkey_Handler,

    KeyboardHandler,
    Breakkey_Handler,

    KeyboardHandler,
    Breakkey_Handler,
};

void Irq_Handler() {
    gKern.in_irq = gTRUE;
    SpinIrq();

    // Clear the VSYNC IRQ by reading PortB output register.
    gword const clear_irq = Pia0PortB;
    (void) gPeek1(clear_irq);

    Real_IncrementTicks();
    Breakkey_Handler();

    gAssert(gReal.ticks < 6);
    if (gKern.focus_game) {
// Console_Printf("F");
        Irq_FocusGameSchedule[gReal.ticks]();
    } else if (gKern.in_game) {
        Irq_PassiveGameSchedule[gReal.ticks]();
    } else {
// Console_Printf("$");
        Irq_FocusShellSchedule[gReal.ticks]();
    }

    gKern.in_irq = gFALSE;
}

void Irq_Handler_Wrapper() {
    asm volatile("\n"
        "  .globl _Irq_Handler_entry  \n"
        "_Irq_Handler_entry:  \n"
        "  JSR _Irq_Handler  \n"
        "  RTI  \n");
    gPoke2(0, Irq_Handler);
}
