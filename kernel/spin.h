// Spinors are just for visualization and debugging.
// They create a "spinning" character on the Console top bar.
// They should be of no consequence.

inline void Spin(byte b) { Cons[b]++; }

inline void SpinNoGameTask() { Spin(1); }
inline void SpinCWait() { Spin(2); }

inline void SpinIrq() { Spin(20); }
inline void SpinRealSeconds() { Spin(28); }
inline void SpinBreakkey() { Spin(30); }
