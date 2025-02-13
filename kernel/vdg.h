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

// Reserved for Chat Mode.
#define Cons 0x0200
#define Disp 0x0400

void Vdg_RestoreGameMode();
void Vdg_Init(void);

