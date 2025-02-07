void main() {
    Poke1(IRQVEC, JMP_Extended);
    Poke2(IRQVEC+1, Irq_Handler_Wrapper);
    Poke2(IRQVEC+1, Irq_Handler_RTI);
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
