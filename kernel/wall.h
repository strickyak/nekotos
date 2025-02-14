// Wall Time 

// Wall Time is meant for displaying clocks and weather and the like.
//
// Wall Time might be adjusted, even backwards.
// Different cocos may use different time zones,
// and therefore have different Wall Times.

struct wall {
    byte second; // 0 to 59
    byte minute; // 0 to 59
    byte hour;  // 0 to 23

    byte day;   // 1 to 31
    byte month; // 1 to 12
    byte year2000;  // e.g. 25 means 2025
    byte dow[3];  // e.g. Mon
    byte moy[3];  // e.g. Jan

    // If hour rolls over,
    // these values are copied to day, month, etc.
    byte next_day;
    byte next_month;
    byte next_year2000;
    byte next_dow[3];
    byte next_moy[3];
}; // Instance is named Wall.

void Wall_IncrementSecond(void);
