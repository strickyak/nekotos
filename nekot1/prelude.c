#include "nekot1/private.h"

void gMemcpy(void *dest, const void *src, gword count) {
    gbyte* d1 = (gbyte*)dest;
    gbyte* s1 = (gbyte*)src;
    // If count is odd, copy one initial byte.
    if (count & 1) {
        *d1++ = *s1++;
    }
    // Now use a stride of 2 bytes.
    count >>= 1;  // Divide count by 2.
    gword* d2 = (gword*)d1;
    gword* s2 = (gword*)s1;
    for (gword i = 0; i < count; i++) {
        *d2++ = *s2++;
    }
}

void gMemset(void* dest, gbyte value, gword count) {
    gbyte* d1 = (gbyte*)dest;
    // If count is odd, set one initial byte.
    if (count & 1) {
        *d1++ = value;
    }
    // Now use a stride of 2.
    count >>= 1;  // Divide count by 2.

    gwob w;
    w.b[0] = w.b[1] = value;

    gword* d2 = (gword*)d1;
    for (gword i = 0; i < count; i++) {
        *d2++ = w.w;
    }
}

void* memset(void* dest, int value, size_t count) {
    gbyte* p = dest;
    gbyte v = (gbyte)value;
    for (gword i = 0; i < count; i++) {
        *p++ = v;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    const gbyte* s = src;
    gbyte* p = dest;
    for (gword i = 0; i < count; i++) {
        *p++ = *s++;
    }
    return dest;
}

int strlen(const char* s) {
    const char* p = s;
    while (*p) ++p;
    return p-s;
}
