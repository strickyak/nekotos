#ifndef _g_VDG_H_
#define _g_VDG_H_

struct vdg {
    word  game_mode;
    byte* game_framebuffer;
    byte shadow_pia1portb;
} Vdg;

// These are for setting the Game Mode.
// public
void gGameShowsTextScreen(byte* screen_addr, byte colorset);
// public
void gGameShowsPMode1Screen(byte* screen_addr, byte colorset);

// gGameShowsOtherScreen requests game modes other
// Text and PMode1.  You must know the high bits
// written to the VDG via the top five bits 
// of $FF22, and the three V2, V1, V0
// "FFC0 - FFC5 Video display mode" bits for the SAM chip.
// Mode_code contains both of those.
// The top five bits of $FF22 are the top five bits
// (in the high byte) of mode_code.
// The V2, V1, V0 bits are the low three bits
// (in the low byte) of mode_code.
// public
void gGameShowsOtherScreen(byte* screen_addr, word mode_code);

#define COLORSET_GREEN  0
#define COLORSET_ORANGE 1

// Used by the kernel:
void SwitchToChatScreen();
void SwitchToGameScreen();
void Vdg_Init(void);

#endif // _g_VDG_H_
