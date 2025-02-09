struct vdg {
#define GM_Text   0
#define GM_PMode1 1
    byte game_mode;
    byte game_colorset;
    uint game_framebuffer;
};

// These are for Game Mode.
void Vdg_GameText(word fb, byte colorset);
void Vdg_GamePMode1(word fb, byte colorset);
// For the "fb" parameter:
#define Graf 0x3000  // Reserved fb for PMode1 graphics.
#define Text 0x3C00  // Reserved fb for Text.

// Reserved for Chat Mode.
#define Con 0x3E00

void Vdg_RestoreGameMode();
void Vdg_Init(void);

