#include "n1/private.h"

#define  W  N1Wall

static void IncrementDay() {
    // Rather than do Gregorian Date calculations,
    // we let the server tell us what the next day is.
    W.day = W.next_day;  
    W.month = W.next_month;  
    W.year2000 = W.next_year2000;  
    MemCopy((byte*)W.dow, W.next_dow, 3);
    MemCopy((byte*)W.moy, W.next_moy, 3);
}

static void IncrementHour() {
    if (W.hour < 23) {
        ++W.hour;
    } else {
        W.hour = 0;
        IncrementDay();
    }
}

static void IncrementMinute() {
    if (W.minute < 59) {
        ++W.minute;
    } else {
        W.minute = 0;
        IncrementHour();
    }
}

void Wall_IncrementSecond() {
    if (W.second < 59) {
        ++W.second;
    } else {
        W.second = 0;
        IncrementMinute();
    }
}
