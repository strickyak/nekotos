extern void Spacewar_Main(void);

void Fatal(const char* why, word arg) {
    Vdg_Init();

    const char* s = why;
    uint const vdg = 0x200;
    while (1) {
        for (uint i = 0; i < 512; i++) {
            Poke1(vdg+i, *s++);
            if (!*s) s = why;
        }
    }
}

void Main_Main() {
    // Display letters on initial pages, so we can tell
    // which page the VDG is showing.  Except page 0x0100
    // is .bss so it must be zeroed.
#if 0
    for (uint i = 0; i < 16; i++) {
        uint page = (i<<8);
        byte value = (i==1) ? 0 : i+64; // Page 1 is BSS.
        for (uint j = 0; j < 256; j++) {
            Poke1(page+j, value);
        }
    }
#endif

    // Set the IRQ vector code, if Coco 1 or 2.
    Poke1(IRQVEC_COCO12, JMP_Extended);
    Poke2(IRQVEC_COCO12+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO12+1, Irq_Handler_RTI);

    // Set the IRQ vector code, if Coco 3.
    Poke1(IRQVEC_COCO3, JMP_Extended);
    Poke2(IRQVEC_COCO3+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO3+1, Irq_Handler_RTI);


    Vdg_Init();
    Console_Init();

    Poke2(0, PutStr);
    Poke2(0, PutChar);

    Printf("Hello %s!\n", "World");
    PutChar('1');
    PutChar('2');
    PutChar('\n');
    PutChar('3');
    PutChar('\n');
    for (uint i=400; i < 410; i++) {
        Printf("(%x=%d.) ", i, i);
    }

    Network_Init();

    Spacewar_Main();
    Fatal("EXIT", 0);
}


int main() {
  Poke2(0, (word)Main_Main);
  asm("  lds #$4000\n"  // To fit in 16K RAM.
      "  ldu #0\n"      // Initial NULL frame pointer.
      "  clrb\n"
      "  tfr b,dp\n"    // Direct page is zero page.
      "  orcc #$50\n"   // No IRQs, FIRQs, for now.
      "  jsr _Main_Main\n");  // GOTO Main_Main above.
}

#if 0

5117 BFF2 0100             (          bas.asm):04499         LBFF2   FDB     SW3VEC  SWI3
5118 BFF4 0103             (          bas.asm):04500         LBFF4   FDB     SW2VEC  SWI2
5119 BFF6 010F             (          bas.asm):04501         LBFF6   FDB     FRQVEC  FIRQ
5120 BFF8 010C             (          bas.asm):04502         LBFF8   FDB     IRQVEC  IRQ
5121 BFFA 0106             (          bas.asm):04503         LBFFA   FDB     SWIVEC  SWI
5122 BFFC 0109             (          bas.asm):04504         LBFFC   FDB     NMIVEC  NMI
5123 BFFE A027             (          bas.asm):04505         LBFFE   FDB     RESVEC  RESET

#endif
