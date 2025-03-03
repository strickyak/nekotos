#ifndef _N1_VDG_H_
#define _N1_VDG_H_

struct vdg {
#define GM_Text   0
#define GM_PMode1 1
    byte game_mode;
    byte game_colorset;
    byte* game_framebuffer;
} Vdg;

// These are for setting the Game Mode.
void N1TextModeForGame(byte* screen_addr, byte colorset);
void N1PMode1ForGame(byte* screen_addr, byte colorset);
void N1ModeForGame(byte* screen_addr, word mode_code);

#define COLORSET_GREEN  0
#define COLORSET_ORANGE 1

// Used by the kernel:
void Vdg_SetConsoleMode();
void Vdg_RestoreGameMode();
void Vdg_Init(void);

#endif // _N1_VDG_H_
