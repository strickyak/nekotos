


void Spacewar_Main() {
    Vdg_TextMode(0x200, 0);

    asm volatile("  andcc #^$50"); // Allow interrupts

    byte* p = 0x300;
    while (true) {
        p[0]++;
        if (!p[0]) {
            p[2]++;
            if (!p[2]) {
                p[4]++;
            }
        }
    }
}
