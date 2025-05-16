#ifndef _NEKOTOS_LIB_MUSIC_H_
#define _NEKOTOS_LIB_MUSIC_H_

#define NUM_VOICES 2

struct tone {
    gword ttl;
    gword stride;
    gword* wave_table;
};

struct voice {
    gword ttl;
    struct tone* tone;
    gword phase;
} Voice[NUM_VOICES];

void MusicInit(byte voice, struct tone* tones) {
    struct voice* v = Voice+voice;
    v->tone = tones;
    v->ttl = tones->ttl;
}

void MusicStep() {
    gword volt = 0x8000;
    for (voice* v = Voices;
            v < Voices+NUM_VOICES;
            v++) {
        struct tone* t = v->tone;


        // If this one tone if finished
        if (!v->ttl) {
            t++;
            // If there are no more tones.
            if (not t->ttl) continue;

            // Set the next tone in the voice.
            v->ttl = t->ttl;
            v->tone = t;
        }

        // Use high byte of phase
        byte sub = gPeek1(&phase) >> 2;
        volt += t->wave_table[sub];
        v->phase += t->stride;
        -- v->ttl;
    }

    // Use high byte of volt
    gPoke1(0xFF20, 0xFC & gPeek1(&volt));
}

#endif //  _NEKOTOS_LIB_MUSIC_H_
