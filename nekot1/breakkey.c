#include "nekot1/private.h"

static void DoBreak(void) {
    if (gKern.focus_game) {
        // FOCUS IS IN GAME.
        // Switch focus to chat.
        gKern.focus_game = gFALSE;
        SwitchToChatScreen();
    } else {
        // FOCUS IS IN CHAT.
        if (gKern.in_game) {
            // Switch focus to game.
            gKern.focus_game = gTRUE;
            SwitchToGameScreen();
        } else {
            // Probaby already in chat, but force it again.
            gKern.focus_game = gFALSE;
            SwitchToChatScreen();
        }
    }
}

// For Coco1, 2, or 3 keyboard.
#define BREAKKEY_PROBE_BIT 0x04
#define BREAKKEY_SENSE_BIT 0x40

// Break_Handler is called on interrupt.
void Breakkey_Handler(void) {
    // Place the keyboard probe signal for BREAK
    const gbyte probe = ~(gbyte)BREAKKEY_PROBE_BIT;
    gPoke1(Pia0PortB, probe);

    // Read the sense port and check the bit.
    gbyte sense = gPeek1(Pia0PortA);

    // Debounce.
    if ((sense & BREAKKEY_SENSE_BIT) == 0) {
        // BREAK KEY DOWN
        if (!Breakkey.break_key_was_down) {
            Breakkey.break_key_was_down = gTRUE;
            DoBreak();
        }
        SpinBreakkey();
    } else {
        // BREAK KEY UP
        Breakkey.break_key_was_down = gFALSE;
    }
}
