#include "nekot1/private.h"

void MemCopy(gbyte *dest, const gbyte *src, gword count) {
    // If count is odd, copy an initial gbyte.
    if (count & 1) {
        *dest++ = *src++;
    }
    // Now use a stride of 2.
    count >>= 1;  // Divide count by 2.
    gword* d = (gword*)dest;
    gword* s = (gword*)src;
    for (gword i = 0; i < count; i++) {
        *d++ = *s++;
    }
}

void MemSet(gbyte* dest, gbyte value, gword count) {
    // If count is odd, set an initial gbyte.
    if (count & 1) {
        *dest++ = value;
    }
    // Now use a stride of 2.
    count >>= 1;  // Divide count by 2.

    gwob w;
    w.b[0] = w.b[1] = value;

    gword* d = (gword*)dest;
    for (gword i = 0; i < count; i++) {
        *d++ = w.w;
    }
}

void* memset(void* dest, int value, gword count) {
    gbyte* p = dest;
    gbyte v = (gbyte)value;
    for (gword i = 0; i < count; i++) {
        *p++ = v;
    }
}

void* memcpy(void* dest, void* src, gword count) {
    gbyte* s = src;
    gbyte* p = dest;
    for (gword i = 0; i < count; i++) {
        *p++ = *s++;
    }
}
