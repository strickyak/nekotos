void TextAt0200() {
    // Video Dispaly Mode: Text (512)
    Poke1(0xFFC0, 0);
    Poke1(0xFFC2, 0);
    Poke1(0xFFC4, 0);
    // Frame buffer at $0200
    Poke1(0xFFC7, 0); // 1
    Poke1(0xFFC8, 0);
    Poke1(0xFFCA, 0);
    Poke1(0xFFCC, 0);
    Poke1(0xFFCE, 0);
    Poke1(0xFFD0, 0);
    Poke1(0xFFD2, 0);
    // Slow CPU
    Poke1(0xFFD6, 0);
    Poke1(0xFFD8, 0);

    // PUT THE VDG INTO ALPHA-GRAPHICS MODE
    Poke1(0xFF22, 0x07);
}

void Vdg_TextMode(uint addr, byte css) {
    TextAt0200();
}

void Vdg_Init() {
    TextAt0200();
}
