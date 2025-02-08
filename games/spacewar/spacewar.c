


void Spacewar_Main() {
    // Vdg_TextMode(0x200, 0);

    asm volatile("  andcc #^$50"); // Allow interrupts

    volatile byte* p = 0x300;
    while (true) {
        p[0]+=1;
        if (!p[0]) {
            p[1]+=1;
            if (!p[1]) {
                p[2]+=1;
            }
        }
    }
}
