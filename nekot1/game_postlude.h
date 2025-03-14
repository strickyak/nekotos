// nekot1/game_postlude.h {

// You shall have no other mains before Me.
int main() {
    asm volatile(".globl __n1pre_entry");
    gPoke2(0, &_n1pre_entry);

    asm volatile(".globl __n1pre_final");
    gPoke2(0, &_n1pre_final);

    asm volatile(".globl __n1pre_final_startup");
    gPoke2(0, &_n1pre_final_startup);

    setup();

    xAfterSetup(loop, &_n1pre_final, &_n1pre_final_startup);
}

int const _n1pre_screens = _nPRE_SCREENS;
int const _n1pre_regions = _nPRE_REGIONS;

// Putting these 3 bytes first in a game's .DECB file
// might simplify finding the entry?
// TODO: does this really help?
struct _n1pre_entry const _n1pre_entry
    __attribute__ ((section (".text.entry"))) = {
        0x7E,  // = JMP extended
        (int)main,
};

// } END nekot1/game_postlude.h
