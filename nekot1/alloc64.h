#ifndef _N1_ALLOC64_H_
#define _N1_ALLOC64_H_

// N1Alloc64 returns a pointer to a 64-byte
// chunk of memory, or it returns NULL
// (the zero pointer).  You can assert the
// result is not NULL using assert():
//    byte* p = N1Alloc64();
//    assert(p);
byte* N1Alloc64();

// N1Free64 frees a 64-byte chunk of memory
// perviously allocated by N1Alloc64.
void N1Free64(byte* ptr);

void Reset64();

void Alloc64_Init();

#endif // _N1_ALLOC64_H_
