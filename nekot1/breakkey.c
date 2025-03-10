#include "nekot1/private.h"

static void DoBreak(void) {
    if (Kern.focus_game) {
        // FOCUS IS IN GAME.
        // Switch focus to chat.
        Kern.focus_game = false;
        SwitchToChatScreen();
    } else {
        // FOCUS IS IN CHAT.
        if (Kern.in_game) {
            // Switch focus to game.
            Kern.focus_game = true;
            SwitchToGameScreen();
        } else {
            // Probaby already in chat, but force it again.
            Kern.focus_game = false;
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
    Poke1(Pia0PortB, probe);

    // Read the sense port and check the bit.
    gbyte sense = Peek1(Pia0PortA);

    // Debounce.
    if ((sense & BREAKKEY_SENSE_BIT) == 0) {
        // BREAK KEY DOWN
        if (!Breakkey.break_key_was_down) {
            Breakkey.break_key_was_down = true;
            DoBreak();
        }
        SpinBreakkey();
    } else {
        // BREAK KEY UP
        Breakkey.break_key_was_down = false;
    }
}
