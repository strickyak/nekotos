struct vdg {
#define GM_Text   0
#define GM_PMode1 1
    byte game_mode;
    byte game_colorset;
    vptr game_framebuffer;
};

// These are for setting the Game Mode.
void Vdg_GameText(vptr fb, byte colorset);
void Vdg_GamePMode1(vptr fb, byte colorset);

#define COLORSET_GREEN  0
#define COLORSET_ORANGE 1

// Used by the kernel:
void Vdg_RestoreGameMode();
void Vdg_Init(void);
