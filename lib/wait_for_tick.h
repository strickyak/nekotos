#ifndef _NEKOTOS_LIB_WAIT_FOR_TICK_H_
#define _NEKOTOS_LIB_WAIT_FOR_TICK_H_

void WaitFor60HzTick() {
    gbyte t = gPeek1(&gMono.ticks);
    while (gPeek1(&gMono.ticks) == t) {}
}

void WaitFor10HzTick() {
    gbyte t = gPeek1(&gMono.decis);
    while (gPeek1(&gMono.decis) == t) {}
}

void WaitFor1HzTick() {
    gbyte t = gPeek1(&gMono.seconds);
    while (gPeek1(&gMono.seconds) == t) {}
}

#endif // _NEKOTOS_LIB_WAIT_FOR_TICK_H_
