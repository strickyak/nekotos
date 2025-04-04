#include "kernel/private.h"

#define MAGIC64 0x1EE1

typedef struct header64 {
    gword magic;
    struct header64* next;
} H64;

H64* root64;

void* gAlloc64() {
    H64* ptr = gNULL;
    gbyte cc_value = gIrqSaveAndDisable();

    if (root64) {
        gAssert(root64->magic == MAGIC64);
        ptr = root64;
        root64 = root64->next;
        ptr->magic = 0;
    }

    gIrqRestore(cc_value);
    return ptr;
}

void gFree64(void* ptr) {
    if (!ptr) return;
    H64* h = (H64*) ptr;

    gbyte cc_value = gIrqSaveAndDisable();

    h->next = root64;
    h->magic = MAGIC64;
    root64 = h;

    gIrqRestore(cc_value);
}

void Reset64() {
    root64 = gNULL;
}

void Alloc64_Init() {
    memset_words(0x0200, 0x3F3F, 256);
    Reset64();
     for (gword p = 0x0200; p < 0x0400; p+=64) {
        gFree64((void*)p);
     }
}
