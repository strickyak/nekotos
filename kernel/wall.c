#include "kernel/private.h"

struct wall gWall gZEROED;

#define  W  gWall

static void Wall_IncrementDay() {
    // Rather than do Gregorian Date calculations,
    // we let the server tell us what the next day is.
    W.day = W.next_day;  
    W.month = W.next_month;  
    W.year2000 = W.next_year2000;  
    memcpy((gbyte*)W.dow, W.next_dow, 3);
    memcpy((gbyte*)W.moy, W.next_moy, 3);
}

static void Wall_IncrementHour() {
    if (W.hour < 23) {
        ++W.hour;
    } else {
        W.hour = 0;
        Wall_IncrementDay();
    }
}

static void Wall_IncrementMinute() {
    if (W.minute < 59) {
        ++W.minute;
    } else {
        W.minute = 0;
        Wall_IncrementHour();
    }
}

void Wall_IncrementSecond() {
    if (W.second < 59) {
        ++W.second;
    } else {
        W.second = 0;
        Wall_IncrementMinute();
    }
}

#undef  W
