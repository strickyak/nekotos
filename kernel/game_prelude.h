// kernel/game_prelude.h {

unsigned int _n1pre_final           __attribute__ ((section (".final"))) = 0xAEEE;
unsigned int _n1pre_final_startup   __attribute__ ((section (".final.setup"))) = 0xAEEF;

extern struct _n1pre_entry {char e; int m;} const _n1pre_entry;

extern void setup(void);
extern void loop(void);
extern void xAfterSetup(void (*loop)(void), unsigned int* final_, unsigned int* final_startup);

#include "kernel/public.h"

// } END kernel/game_prelude.h
