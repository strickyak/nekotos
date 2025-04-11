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
  H64* h = (H64*)ptr;

  gbyte cc_value = gIrqSaveAndDisable();

  h->next = root64;
  h->magic = MAGIC64;
  root64 = h;

  gIrqRestore(cc_value);
}

void Reset64() { root64 = gNULL; }

// Chunks only exist while in a game.
// When no game, this space is in an unknown state.
// (There may have been chunks allocated when the game died.)
void Alloc64_Init() {
  memset_words(CHUNKS_BEGIN, 0x3F3F, (CHUNKS_LIMIT - CHUNKS_BEGIN) / 2);
  Reset64();
  for (gword p = CHUNKS_BEGIN; p < CHUNKS_LIMIT; p += 64) {
    gFree64((void*)p);
  }
}
