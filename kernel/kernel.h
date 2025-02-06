typedef void (*func)(void);

struct kernel {
    func irq_func; 
    func firq_func; 
    func nmi_func; 

    func tick_func; 
    func second_func; 

    byte sub_ticks;
    byte ticks;
    uint seconds;

    byte wall_second;
    byte wall_minute;
    byte wall_hour;
    byte wall_day;
    byte wall_month;
    byte wall_year2000;
    char dow[3];
    char moy[3];

    byte next_day;
    byte next_month;
    byte next_year2000;
    char next_dow[3];
    char next_moy[3];
};
