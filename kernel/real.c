static void IncrementSeconds() {
    ++Real.seconds;

    // Now go increment the Wall Time.
    Wall_IncrementSecond();

    // Show on console
    volatile word* p = 0x300;
    p[8]++;
}

static void IncrementDecis() {
    if (Real.decis < 9) {
        ++Real.decis;
    } else {
        Real.decis = 0;
        IncrementSeconds();
    }
}

void Real_IncrementTicks() {
    if (Real.ticks < 5) {
        ++Real.ticks;
    } else {
        Real.ticks = 0;
        IncrementDecis();
    }
}
