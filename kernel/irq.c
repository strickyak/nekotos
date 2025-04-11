#include "kernel/private.h"

void Network_Handler(void);
void KeyboardHandler(void);

void NOOP() {}

gfunc Irq_FocusGameSchedule[6] = {
    Network_Handler, Breakkey_Handler,

    Network_Handler, Breakkey_Handler,

    Network_Handler, Breakkey_Handler,
};

gfunc Irq_PassiveGameSchedule[6] = {
    Network_Handler, Breakkey_Handler,

    Network_Handler, KeyboardHandler,

    Network_Handler, KeyboardHandler,
};

gfunc Irq_FocusShellSchedule[6] = {
    KeyboardHandler, Breakkey_Handler,

    KeyboardHandler, Breakkey_Handler,

    KeyboardHandler, Breakkey_Handler,
};

void CheckLamb() {
  if (Vdg.sacrificial_lamb != 123) {
    gFatal("LAMB", Vdg.sacrificial_lamb);
  }
}

void Irq_Handler() {
  // Clear the VSYNC IRQ by reading PortB output register.
  (void)gPeek1(Pia0PortB);

  Mono_IncrementTicks();

  if (gKern.focus_game) {
    Irq_FocusGameSchedule[gMono.ticks]();
  } else if (gKern.in_game) {
    Irq_PassiveGameSchedule[gMono.ticks]();
  } else {
    Irq_FocusShellSchedule[gMono.ticks]();
  }
}

void Irq_Handler_Wrapper() {
  asm volatile(
      "\n"
      "  .globl _Irq_Handler_entry  \n"
      "_Irq_Handler_entry:  \n"
      "  JSR _Irq_Handler  \n"
      "  RTI  \n");
  gPoke2(0, Irq_Handler);
}
