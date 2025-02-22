static void IncrementDay() {
    // Rather than do Gregorian Date calculations,
    // we let the server tell us what the next day is.
    Wall.day = Wall.next_day;  
    Wall.month = Wall.next_month;  
    Wall.year2000 = Wall.next_year2000;  
    MemCopy((byte*)Wall.dow, Wall.next_dow, 3);
    MemCopy((byte*)Wall.moy, Wall.next_moy, 3);
}

static void IncrementHour() {
    if (Wall.hour < 23) {
        ++Wall.hour;
    } else {
        Wall.hour = 0;
        IncrementDay();
    }
}

static void IncrementMinute() {
    if (Wall.minute < 59) {
        ++Wall.minute;
    } else {
        Wall.minute = 0;
        IncrementHour();
    }
}

void Wall_IncrementSecond() {
    if (Wall.second < 59) {
        ++Wall.second;
    } else {
        Wall.second = 0;
        IncrementMinute();
    }
}
