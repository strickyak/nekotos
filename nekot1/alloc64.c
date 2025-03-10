#include "nekot1/private.h"

#define MAGIC64 0x1EE1

typedef struct header64 {
    word magic;
    struct header64* next;
} H64;

H64* root64;

byte* gAlloc64() {
    H64* z = NULL;
    byte cc_value = gIrqSaveAndDisable();

    if (root64) {
        assert(root64->magic == MAGIC64);
        z = root64;
        root64 = root64->next;
        z->magic = 0;
    }

    gIrqRestore(cc_value);
    return (byte*)z;
}

void gFree64(byte* p) {
    if (!p) return;
    H64* h = (H64*) p;

    byte cc_value = gIrqSaveAndDisable();

    h->next = root64;
    h->magic = MAGIC64;
    root64 = h;

    gIrqRestore(cc_value);
}

void Reset64() {
    root64 = NULL;
}
