#ifndef _KERNEL_VDG_H_
#define _KERNEL_VDG_H_

extern struct vdg {
    gbyte  sacrificial_lamb;  // Something keeps clearing $3F.
    gbyte  game_vdg_mode;
    gbyte  game_sam_mode;
    gbyte* game_framebuffer;
    gbyte shadow_pia1portb;
    gbool  stiff_debug;
} Vdg;

// These are for setting the Game Mode.
void gTextScreen(gbyte* screen_addr, gbyte colorset);
void gPMode1Screen(gbyte* screen_addr, gbyte colorset);

// gModeScreen requests game modes other
// Text and PMode1.  You must know the high bits
// written to the VDG via the top five bits 
// of $FF22, and the three V2, V1, V0
// "FFC0 - FFC5 Video display mode" bits for the SAM chip.
void gModeScreen(gbyte* screen_addr, gbyte vdg_mode, gbyte sam_mode);

#define COLORSET_GREEN  0
#define COLORSET_ORANGE 1

// Used by the kernel:
void NowSwitchToChatScreen();
void NowSwitchToGameScreen();
void Vdg_Init(void);

// For CLEAR key for stiff_debug.
void NowSwitchDisplayMode(gbyte* fb, gbyte vdg_mode, gbyte sam_mode);

#endif // _KERNEL_VDG_H_
