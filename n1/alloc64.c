#include "n1/private.h"

#define MAGIC64 0x1EE1

typedef struct header64 {
    word magic;
    struct header64* next;
} H64;

H64* root64;

byte* N1Alloc64() {
    H64* z = NULL;
    byte cc_value = N1IrqSaveAndDisable();

    if (root64) {
        assert(root64->magic == MAGIC64);
        z = (byte*)root64;
        root64 = root64->next;
        z->magic = 0;
    }

    N1IrqRestore(cc_value);
    return (byte*)z;
}

void N1Free64(byte* p) {
    if (!p) return;
    H64* h = (H64*) p;

    byte cc_value = N1IrqSaveAndDisable();

    h->next = root64;
    h->magic = MAGIC64;
    root64 = h;

    N1IrqRestore(cc_value);
}

void Reset64() {
    root64 = NULL;
}
