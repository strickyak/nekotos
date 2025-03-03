#include "n1/private.h"

static void DoBreak(void) {
    // TODO
}

// For Coco1, 2, or 3 keyboard.
#define BREAKKEY_PROBE_BIT 0x04
#define BREAKKEY_SENSE_BIT 0x40

// Break_Handler is called on interrupt.
void Breakkey_Handler(void) {
    // Place the keyboard probe signal for BREAK
    const byte probe = ~(byte)BREAKKEY_PROBE_BIT;
    Poke1(Pia0PortB, probe);

    // Read the sense port and check the bit.
    byte sense = Peek1(Pia0PortA);
    if ((sense & BREAKKEY_SENSE_BIT) == 0) {  // BREAK?
        DoBreak();
        SpinBreakkey();
    }
}
