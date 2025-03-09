#include "n1/private.h"

// See page 248 in
// "Assembly Language Programming for the Color Computer (1985)(Laurence A Tepolt)(pdf)".
// Our TextMode is Display Mode "AI".
// Our PMode1 is Display Mode "G3C".
//
// The vdg_op_mode are written to $FF22 (Pia1PortB).
// The low three bits can be set high.
//
// See page 3 in "CoCo Hardware Reference.pdf".
// The sam_control_bits are called "FFC0-FFC5 Video Display Mode".

static void SwitchDisplayMode(byte* fb, byte vdg_op_mode, byte sam_control_bits) {
    vdg_op_mode &= 0xF8;  // only top 5 bits matter.

    byte cc_value = N1IrqSaveAndDisable();

    Vdg.shadow_pia1portb = vdg_op_mode;
    Poke1(0xFF22, vdg_op_mode);  // Set VDG bits.

// Console_Printf(" D[%x,%x,%x] ", fb, vdg_op_mode, sam_control_bits);

    // Set the framebuffer address.
    {
        word bit = 0x0200;  // Start with bit F0 (CoCo Hardware Reference.pdf)
        for (byte i=0; i<14; i+=2) {  // 7 iterations.
            bool b = (((word)fb & bit) != 0); 
            Poke1(0xFFC6 + i + b, 0); // 0xFFC6 is F0.
            bit <<= 1;
        }
    }

    // Set the V2, V1, V0 SAM bits.
    {
        byte bit = 0x01;
        for (byte i=0; i<6; i+=2) {  // 3 iterations.
            bool b = ((sam_control_bits & bit) != 0); 
            Poke1(0xFFC0 + i + b, 0); // 0xFFC0 is V0.
            bit <<= 1;
        }
    }

    N1IrqRestore(cc_value);
}
// Effective immediately.
static void SwitchToDisplayText(byte* fb, byte colorset) {
    SwitchDisplayMode(fb, (colorset?8:0), 0);
}
// Effective immediately.
static void SwitchToDisplayPMode1(byte* fb, byte colorset) {
    SwitchDisplayMode(fb, 0xC0 + (colorset?8:0), 4);
}
// Effective immediately.
void SwitchToGameScreen() {
    wob w;
    w.w = Vdg.game_mode;
    SwitchDisplayMode(Vdg.game_framebuffer, w.b[0], w.b[1]);
}

// These are for Game Mode.
// They remember what to change back to, after Chat mode.
void N1GameShowsTextScreen(byte* fb, byte colorset) {
    Vdg.game_mode = (colorset? 0x0800: 0x0000);
    Vdg.game_framebuffer = fb;
    if (Kern.focus_game) SwitchToGameScreen();
}
void N1GameShowsPMode1Screen(byte* fb, byte colorset) {
    Vdg.game_mode = (colorset? 0xC804: 0xC004);
    Vdg.game_framebuffer = fb;
    if (Kern.focus_game) SwitchToGameScreen();
}
void N1GameShowsOtherScreen(byte* fb, word mode_code) {
    Vdg.game_mode = mode_code;
    Vdg.game_framebuffer = fb;
    if (Kern.focus_game) SwitchToGameScreen();
}

void SwitchToChatScreen() {
    SwitchToDisplayText(
        Cons,
        Kern.in_game ? COLORSET_ORANGE : COLORSET_GREEN
    );
}

void Vdg_Init() {
    SwitchToChatScreen();
}
