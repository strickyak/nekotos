#ifndef _g_ALLOC64_H_
#define _g_ALLOC64_H_

// gAlloc64 returns a pointer to a 64-byte
// chunk of memory, or it returns NULL
// (the zero pointer).  You can assert the
// result is not NULL using assert():
//    byte* p = gAlloc64();
//    assert(p);
byte* gAlloc64();

// gFree64 frees a 64-byte chunk of memory
// perviously allocated by gAlloc64.
void gFree64(byte* ptr);

void Reset64();

void Alloc64_Init();

#endif // _g_ALLOC64_H_
