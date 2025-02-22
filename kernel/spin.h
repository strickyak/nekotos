// Spinors are just for visualization and debugging.
// They create a "spinning" character on the Console top bar.
// They should be of no consequence.
//
// The "mark" is a letter just to the left of the spinor
// on the bar, to mnemonically identify the spinor.

inline void Spin(char c, byte b) {
    Cons[b]++;
}
inline void Spin_Mark(char c, byte b) {
    Cons[b-1] = c;
}

inline void SpinNoGameTask() { Spin('N', 14); }
inline void SpinCWait() { Spin('C', 18); }
inline void SpinIrq() { Spin('I', 22); }
inline void SpinRealSeconds() { Spin('S', 26); }
inline void SpinBreakkey() { Spin('B', 30); }

void Spin_Init() {
     Spin_Mark('N', 14);
     Spin_Mark('C', 18);
     Spin_Mark('I', 22);
     Spin_Mark('S', 26);
     Spin_Mark('B', 30);
}

#define WIZNET_BAR_LOCATION 10
