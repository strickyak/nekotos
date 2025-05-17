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
} Voices[NUM_VOICES];

#include "music-gen/music-tables.h"

void MusicInit(gbyte voice, struct tone* tones) {
    struct voice* v = &Voices[voice];
    v->tone = tones;
    v->ttl = tones->ttl;
}

volatile gbyte audio;
volatile gword volt;

__attribute__((always_inline)) inline  void VoiceStep(struct voice* v) {
        struct tone* t = v->tone;

        // If this one tone if finished
        if (!v->ttl) {
            t++;
            // If there are no more tones.
            if (! t->ttl) return;

            // Set the next tone in the voice.
            v->ttl = t->ttl;
            v->tone = t;
        }

        // Use high byte of phase
        // >>1 if the size of the wave table is 128.
#if 0
        gbyte sub = gPeek1(&v->phase) >> 1;
        volt += t->wave_table[sub];
#else
        gbyte offset = gPeek1(&v->phase) & 0xFE;
        volt += gPeek2( offset + (gword)t->wave_table );
#endif
        v->phase += t->stride;
        v->ttl--;
}

void MusicStep() {
    volt = 0x8000;

    VoiceStep( &Voices[0] );
    VoiceStep( &Voices[1] );

    // Use high 6 bits of volt
    gPoke1(0xFF20, 0xFC & gPeek1(&volt));

    // audio += gMono.decis << 3;
    // gPoke1(0xFF20, 0xFC & audio);
}

void MusicSetup() {
    MusicInit(0, Tones0);
    MusicInit(1, Tones1);
}

#endif //  _NEKOTOS_LIB_MUSIC_H_
