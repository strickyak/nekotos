#ifndef _NEKOT1_VDG_H_
#define _NEKOT1_VDG_H_

struct vdg {
    gword  game_mode;
    gbyte* game_framebuffer;
    gbyte shadow_pia1portb;
} Vdg;

// These are for setting the Game Mode.
// public
void gTextScreen(gbyte* screen_addr, gbyte colorset);
// public
void gPMode1Screen(gbyte* screen_addr, gbyte colorset);

// gGameShowsOtherScreen requests game modes other
// Text and PMode1.  You must know the high bits
// written to the VDG via the top five bits 
// of $FF22, and the three V2, V1, V0
// "FFC0 - FFC5 Video display mode" bits for the SAM chip.
// Mode_code contains both of those.
// The top five bits of $FF22 are the top five bits
// (in the high gbyte) of mode_code.
// The V2, V1, V0 bits are the low three bits
// (in the low gbyte) of mode_code.
// public
void gGameShowsOtherScreen(gbyte* screen_addr, gword mode_code);

#define COLORSET_GREEN  0
#define COLORSET_ORANGE 1

// Used by the kernel:
void SwitchToChatScreen();
void SwitchToGameScreen();
void Vdg_Init(void);

#endif // _NEKOT1_VDG_H_
