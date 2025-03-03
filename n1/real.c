#include "n1/private.h"

struct real N1Real;

#define  R  N1Real

static void IncrementSeconds() {
    ++R.seconds;

    // Now go increment the Wall Time.
    Wall_IncrementSecond();

    // Show progress on console.
    SpinRealSeconds();
}

static void IncrementDecis() {
    if (R.decis < 9) {
        ++R.decis;
    } else {
        R.decis = 0;
        IncrementSeconds();
    }
}

void Real_IncrementTicks() {
    if (R.ticks < 5) {
        ++R.ticks;
    } else {
        R.ticks = 0;
        IncrementDecis();
    }
}

#undef R
