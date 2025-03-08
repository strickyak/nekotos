#include "n1/private.h"

void MemCopy(byte *dest, const byte *src, word count) {
    // If count is odd, copy an initial byte.
    if (count & 1) {
        *dest++ = *src++;
    }
    // Now use a stride of 2.
    count >>= 1;  // Divide count by 2.
    word* d = (word*)dest;
    word* s = (word*)src;
    for (word i = 0; i < count; i++) {
        *d++ = *s++;
    }
}

void MemSet(byte* dest, byte value, word count) {
    // If count is odd, set an initial byte.
    if (count & 1) {
        *dest++ = value;
    }
    // Now use a stride of 2.
    count >>= 1;  // Divide count by 2.

    wob w;
    w.b[0] = w.b[1] = value;

    word* d = (word*)dest;
    for (word i = 0; i < count; i++) {
        *d++ = w.w;
    }
}

void* memset(void* dest, int value, word count) {
    byte* p = dest;
    byte v = (byte)value;
    for (word i = 0; i < count; i++) {
        *p++ = v;
    }
}

void* memcpy(void* dest, void* src, word count) {
    byte* s = src;
    byte* p = dest;
    for (word i = 0; i < count; i++) {
        *p++ = *s++;
    }
}
