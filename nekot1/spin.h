#ifndef _NEKOT1_SPIN_H_
#define _NEKOT1_SPIN_H_

// Spinors are just for visualization and debugging.
// They create a "spinning" character on the Console top bar.
// They should be of no consequence.
//
// The "mark" is a letter just to the left of the spinor
// on the bar, to mnemonically identify the spinor.

inline void Spin(char c, gbyte b) {
    Cons[b]++;
    volatile gbyte* p = (volatile gbyte*)0x3FE0;  // last graphics line
    p[b]++;
}   
inline void Spin_Mark(char c, gbyte b) {
    Cons[b-1] = c;
}

inline void SpinChatTask() { Spin('C', 14); }
inline void SpinKeyboardScan() { Spin('K', 18); }

inline void SpinIrq() { Spin('Q', 22); }
inline void SpinRealDecis() { Spin('D', 23); }
inline void SpinRealSeconds() { Spin('S', 24); }

inline void SpinBreakkey() { Spin('B', 30); }

inline void Spin_Init() {
     Spin_Mark('C', 14);
     Spin_Mark('K', 18);
     Spin_Mark('Q', 22);
     Spin_Mark('B', 30);
}

#define WIZNET_BAR_LOCATION 10

#endif // _NEKOT1_SPIN_H_
