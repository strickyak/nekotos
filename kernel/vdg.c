#include "kernel/private.h"
#include "lib/format.h"

struct vdg Vdg gZEROED;

// See page 248 in
// "Assembly Language Programming for the Color Computer (1985)(Laurence A Tepolt)(pdf)".
// Our TextMode is Display Mode "AI".
// Our PMode1 is Display Mode "G3C".
//
// The vdg_mode are written to $FF22 (Pia1PortB).
// The low three bits can be set to 0s.
// (Note that Bit 1 is the 1-Bit Speaker Output.)
//
// See page 3 in "CoCo Hardware Reference.pdf".
// The sam_mode are called "FFC0-FFC5 Video Display Mode".


void Logf(const char* format, ...) {
    char buf[33];
    va_list ap;
    va_start(ap, format);
    Vprintf(buf, format, ap);
    gNetworkLog(buf);
}

gbyte mode_lock;

void NowSwitchDisplayMode(gbyte* fb, gbyte vdg_mode, gbyte sam_mode) {
    if (Vdg.stiff_debug) {
        // Devious hack to lock the VDG Text screen, on CLEAR key.
        fb = Cons;
        vdg_mode = sam_mode = 0;
        gPoke1(Cons+0, 'L');
        gPoke1(Cons+1, 'O');
        gPoke1(Cons+2, 'C');
        gPoke1(Cons+3, 'K');
    }

    vdg_mode &= 0xF8;  // only top 5 bits matter.

/*
    Logf("VDG,%x %x,%x", fb, vdg_mode, sam_mode);
*/
    gbyte cc_value = gIrqSaveAndDisable();

    Vdg.shadow_pia1portb = vdg_mode;
    gPoke1(0xFF22, vdg_mode);  // Set VDG bits.
if(0)PutChar('0' + vdg_mode);

    // Set the framebuffer address.
    {
        gword bit = 0x0200;  // Start with bit F0 (CoCo Hardware Reference.pdf)
        for (gbyte i=0; i<14; i+=2) {  // 7 iterations.
            gbool b = (((gword)fb & bit) != 0); 
            gPoke1(0xFFC6 + i + b, 0); // 0xFFC6 is F0.
            bit <<= 1;
        }
    }

if(0)PutChar('@' + sam_mode);
    // Set the V2, V1, V0 SAM bits.
    {
        gbyte bit = 0x01;
        for (gbyte i=0; i<6; i+=2) {  // 3 iterations.
            gbool b = ((sam_mode & bit) != 0); 
            gPoke1(0xFFC0 + i + b, 0); // 0xFFC0 is V0.
            bit <<= 1;
        }
    }
if(0)PutChar('X');
    gIrqRestore(cc_value);
if(0)PutChar('Y');
}
// Effective immediately.
void NowSwitchToDisplayText(gbyte* fb, gbyte colorset) {
if(0)PutChar('T');
    NowSwitchDisplayMode(fb, /*vdg_mode=*/(colorset?8:0), /*sam_mode=*/0);
}
// Effective immediately.
void NowSwitchToGameScreen() {
if(0)PutChar('V');
    NowSwitchDisplayMode(Vdg.game_framebuffer, Vdg.game_vdg_mode, Vdg.game_sam_mode);
}

// These are for Game Mode.
// They remember what to change back to, after Chat mode.
void gTextScreen(gbyte* fb, gbyte colorset) {
if(0)PutChar('+');
    Vdg.game_vdg_mode = (colorset? 0x08: 0x00);
    Vdg.game_sam_mode = 0x00;
    Vdg.game_framebuffer = fb;
    if (gKern.focus_game) NowSwitchToGameScreen();
}
void gPMode1Screen(gbyte* fb, gbyte colorset) {
if(0)PutChar('#');
    Vdg.game_vdg_mode = (colorset? 0xC8: 0xC0);
    Vdg.game_sam_mode = 0x04;
    Vdg.game_framebuffer = fb;
    mode_lock = Vdg.game_vdg_mode;
    if (gKern.focus_game) NowSwitchToGameScreen();
}
void gModeScreen(gbyte* fb, gbyte vdg_mode, gbyte sam_mode) {
if(0)PutChar('=');
    Vdg.game_vdg_mode = vdg_mode;
    Vdg.game_sam_mode = sam_mode;
    Vdg.game_framebuffer = fb;
    if (gKern.focus_game) NowSwitchToGameScreen();
}

void NowSwitchToChatScreen() {
if(0)PutChar('U');
    NowSwitchToDisplayText(
        Cons,
        gKern.in_game ? COLORSET_ORANGE : COLORSET_GREEN
    );
}

void Vdg_Init() {
    NowSwitchToChatScreen();
}
