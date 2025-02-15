extern void Spacewar_Main(void);

// This is a "Null Game" that does nothing.
void CWait(void) {
    ALLOW_IRQ();

    while (1) {
        // wait for any interrupt.
        asm volatile ("cwai #$00");
        SpinCWait();
    }

#if 0
    // Go into a loop where we just wait on interrupts.
    asm volatile ("   \n"
        "CWait:       \n"
        "  cwai  #$00 \n"  // wait for any interrupt.
        "  inc  $0202 \n"  // First byte of Kernel Console.
        "  jmp CWait  \n"
        );
#endif
}

void InitCoco3() {
#if WUT
    Poke1(0xFF90, 0x80); // Coco 1/2 Compatible, no MMU, no GIME F/IRQs.
    for (word i = 0xFF91; i < 0xFFA0; i++) {
        Poke1(i, 0x00); // Nothing special.
    }
    Poke1(0xFF9C, 0x0F); // Important Fix
    Poke1(0xFF9D, 0xE0); // $FF9[DE] <- $E000

    // Now init the SAM.
    // Poke all the even addresses
    for (word i = 0xFFC0; i< 0xFFE0; i+=2) {
        Poke1(i, 0);
    }
    // Odd exceptions:
    Poke1(0xFFDB, 0);  // Say we have 16K RAM
    Poke1(0xFFC7, 0);  // move up to $0200 for Text Display

    // Now PIA1 controling the VDG.
    Poke1(0xFF21, 0);     // choose direction reg, A port.
    Poke1(0xFF20, 0xFE);  // set direction
    Poke1(0xFF23, 0);     // choose direction reg, B port.
    Poke1(0xFF22, 0xF8);  // set direction

    Poke1(0xFF21, 0x34);  // choose data reg, A port, and CA2 outputs 0.
    Poke1(0xFF23, 0x34);  // choose data reg, B port, and CB2 outputs 0.

    Poke1(0xFF20, 0x02);  // set data (the "2" bit is RS232 out.
    Poke1(0xFF22, 0x00);  // set data

    // And PIA0.
    Poke1(0xFF01, 0);     // choose direction reg, A port.
    Poke1(0xFF00, 0x00);  // set direction, all inputs.
    Poke1(0xFF03, 0);     // choose direction reg, B port.
    Poke1(0xFF02, 0xFF);  // set direction, all outputs.

    Poke1(0xFF01, 0x34);  // choose data reg, A port, and CA2 outputs 0.
    Poke1(0xFF03, 0x34);  // choose data reg, B port, and CB2 outputs 0.
    Poke1(0xFF02, 0xFF);  // null output to keyboard on Port B.
#endif
}

void Fatal(const char* why, word arg) {
    SetConsoleMode();

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
    // Set the IRQ vector code, for Coco 1 or 2.
    Poke1(IRQVEC_COCO12, JMP_Extended);
    Poke2(IRQVEC_COCO12+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC_COCO12+1, Irq_Handler_RTI);

    // Set the IRQ vector code, for Coco 3.
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
    for (uint i=400; i < 404; i++) {
        Printf("(%x=%d.) ", i, i);
    }

    Peek1(0xFF02);
    Poke1(0xFF03, 0x35);  // +1: Enable VSYNC (FS) IRQ
    Peek1(0xFF02);
    ALLOW_IRQ();

    for (word w = 0;w<0xFF00; w++) {
        Poke2(0x208, w);
    }
    for (byte w = 0; w < 128; w++) {
        Poke1(0x240 + w, 128+w);
    }

    Network_Init();
    // CWait();
    Spacewar_Main();
}

void ClearPage0() {
    for (word p = 0; p<256; p+=2) {
        Poke2(p, 0);
    }
}

word PinDown[] = {
    (word) ClearPage0,
    (word) InitCoco3,
    (word) Main_Main,

    (word) Breakkey_Handler,
    (word) Irq_Handler,
    (word) Irq_Handler_RTI,
    (word) Irq_Handler_Wrapper,
    (word) irq_schedule,
    (word) Network_Handler,
    (word) Vdg_Init,

    (word) Console_Init,
    (word) Keyboard_Handler,
    (word) Vdg_GameText,
    (word) Vdg_GamePMode1,
    (word) Wiznet_Handler,
    (word) Network_Handler,
    (word) Wiznet_Init,
    (word) Network_Init,
    (word) CWait,
};

int main() {
  Poke2(0, (word)PinDown);

  asm volatile("\n"
      "  orcc #$50\n"   // No IRQs, FIRQs, for now.
      "  lds #$01FE\n"  // Reset the stack
      "  clra\n"
      "  clrb\n"
      "  tfr d,u\n"     // Initial NULL frame pointer.
      "  tfr b,dp\n"    // Direct page is zero page.
      );
      ClearPage0();
      InitCoco3();
      Main_Main();
      Fatal("EXIT", 0);
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
