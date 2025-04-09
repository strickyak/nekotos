#ifndef _NEKOTOS_LIB_WAIT_FOR_TICK_H_
#define _NEKOTOS_LIB_WAIT_FOR_TICK_H_

void WaitFor60HzTick() {
    gbyte t = gPeek1(&gReal.ticks);
    while (gPeek1(&gReal.ticks) == t) {}
}

void WaitFor10HzTick() {
    gbyte t = gPeek1(&gReal.decis);
    while (gPeek1(&gReal.decis) == t) {}
}

void WaitFor1HzTick() {
    gbyte t = gPeek1(&gReal.seconds);
    while (gPeek1(&gReal.seconds) == t) {}
}

#endif // _NEKOTOS_LIB_WAIT_FOR_TICK_H_
