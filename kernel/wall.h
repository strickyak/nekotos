// Wall Time 

// Wall Time is meant for displaying clocks and weather and the like.
//
// Wall Time might be adjusted, even backwards.
// Different cocos may use different time zones,
// and therefore have different Wall Times.

struct wall {
    byte volatile second; // 0 to 59
    byte volatile minute; // 0 to 59
    byte volatile hour;  // 0 to 23

    byte volatile day;   // 1 to 31
    byte volatile month; // 1 to 12
    byte volatile year2000;  // e.g. 25 means 2025
    byte volatile dow[3];  // e.g. Mon
    byte volatile moy[3];  // e.g. Jan

    // If hour rolls over,
    // these values are copied to day, month, etc.
    byte next_day;
    byte next_month;
    byte next_year2000;
    byte next_dow[3];
    byte next_moy[3];
}; // Instance is named Wall.

void Wall_IncrementSecond(void);
