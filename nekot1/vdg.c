#include "nekot1/private.h"

// See page 248 in
// "Assembly Language Programming for the Color Computer (1985)(Laurence A Tepolt)(pdf)".
// Our TextMode is Display Mode "AI".
// Our PMode1 is Display Mode "G3C".
//
// The vdg_op_mode are written to $FF22 (Pia1PortB).
// The low three bits can be set to 0s.
// (Note that Bit 1 is the 1-Bit Speaker Output.)
//
// See page 3 in "CoCo Hardware Reference.pdf".
// The sam_control_bits are called "FFC0-FFC5 Video Display Mode".

static void NowSwitchDisplayMode(gbyte* fb, gbyte vdg_op_mode, gbyte sam_control_bits) {
    vdg_op_mode &= 0xF8;  // only top 5 bits matter.

    gbyte cc_value = gIrqSaveAndDisable();

    Vdg.shadow_pia1portb = vdg_op_mode;
    gPoke1(0xFF22, vdg_op_mode);  // Set VDG bits.

// Console_Printf(" D[%d,%d,%d] ", fb, vdg_op_mode, sam_control_bits);

    // Set the framebuffer address.
    {
        gword bit = 0x0200;  // Start with bit F0 (CoCo Hardware Reference.pdf)
        for (gbyte i=0; i<14; i+=2) {  // 7 iterations.
            gbool b = (((gword)fb & bit) != 0); 
            gPoke1(0xFFC6 + i + b, 0); // 0xFFC6 is F0.
            bit <<= 1;
        }
    }

    // Set the V2, V1, V0 SAM bits.
    {
        gbyte bit = 0x01;
        for (gbyte i=0; i<6; i+=2) {  // 3 iterations.
            gbool b = ((sam_control_bits & bit) != 0); 
            gPoke1(0xFFC0 + i + b, 0); // 0xFFC0 is V0.
            bit <<= 1;
        }
    }

    gIrqRestore(cc_value);
}
// Effective immediately.
static void NowSwitchToDisplayText(gbyte* fb, gbyte colorset) {
    NowSwitchDisplayMode(fb, (colorset?8:0), 0);
}
// Effective immediately.
void NowSwitchToGameScreen() {
    gwob w;
    w.w = Vdg.game_mode;
    NowSwitchDisplayMode(Vdg.game_framebuffer, w.b[0], w.b[1]);
}

// These are for Game Mode.
// They remember what to change back to, after Chat mode.
void gTextScreen(gbyte* fb, gbyte colorset) {
    Vdg.game_mode = (colorset? 0x0800: 0x0000);
    Vdg.game_framebuffer = fb;
    if (gKern.focus_game) NowSwitchToGameScreen();
}
void gPMode1Screen(gbyte* fb, gbyte colorset) {
    Vdg.game_mode = (colorset? 0xC804: 0xC004);
    Vdg.game_framebuffer = fb;
    if (gKern.focus_game) NowSwitchToGameScreen();
}
void gModeScreen(gbyte* fb, gword mode_code) {
    Vdg.game_mode = mode_code;
    Vdg.game_framebuffer = fb;
    if (gKern.focus_game) NowSwitchToGameScreen();
}

void NowSwitchToChatScreen() {
    NowSwitchToDisplayText(
        Cons,
        gKern.in_game ? COLORSET_ORANGE : COLORSET_GREEN
    );
}

void Vdg_Init() {
    NowSwitchToChatScreen();
    PutChar('V');
}
