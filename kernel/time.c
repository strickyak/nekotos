#include "time.inc"

void wall_day_handler() {
    Time.wall_day = Time.next_day;  
    Time.wall_month = Time.next_month;  
    Time.wall_year2000 = Time.next_year2000;  
    memcpy(Time.dow, Time.next_dow, 3);
    memcpy(Time.moy, Time.next_moy, 3);
}

void wall_hour_handler() {
    if (Time.wall_hour < 23) {
        ++Time.wall_hour;
    } else {
        Time.wall_hour = 0;
        wall_day_handler();
    }
}

void wall_minute_handler() {
    if (Time.wall_minute < 59) {
        ++Time.wall_minute;
    } else {
        Time.wall_minute = 0;
        wall_hour_handler();
    }
}

void wall_second_handler() {
    if (Time.wall_second < 59) {
        ++Time.wall_second;
    } else {
        Time.wall_second = 0;
        wall_minute_handler();
    }
}

void second_handler() {
    ++Time.seconds;  // uint wraps around after 64*1024 seconds.
}

void tick_handler() {
    if (Time.ticks < 9) {
        ++Time.ticks;
    } else {
        Time.ticks = 0;
        Time.second_func();
        wall_second_handler();
    }
}

void time_handler() {
    if (Time.sub_ticks < 5) {
        ++Time.sub_ticks;
    } else {
        Time.sub_ticks = 0;
        Time.tick_func();
    }
}

void time_init() {
    Time.tick_func = tick_handler;
    Time.second_func = second_handler;
}

int main() {
    time_init();
    (volatile void) time_handler();
}
