#include "boot800.inc"

void boot800_init() {
    Boot800.bogus = 0xFFFF;
}

void boot800_main() {
    boot800_init();
}
