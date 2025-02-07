#if 0
void Pia1bOn(byte x) { *(volatile byte*)0xFF22 |= x; }
void Pia1bOff(byte x) { *(volatile byte*)0xFF22 &= ~x; }
void SetOrangeScreen() { Pia1bOn(0x08); }
void SetGreenScreen() { Pia1bOff(0x08); }
#endif

struct vdg {
};
