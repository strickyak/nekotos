#ifndef _NEKOT1_ALLOC64_H_
#define _NEKOT1_ALLOC64_H_

// gAlloc64 returns a pointer to a 64-gbyte
// chunk of memory, or it returns gNULL
// (the zero pointer).  You can gAssert the
// result is not gNULL using gAssert():
//    gbyte* p = gAlloc64();
//    gAssert(p);
void* gAlloc64();

// gFree64 frees a 64-gbyte chunk of memory
// perviously allocated by gAlloc64.
void gFree64(void* ptr);

void Reset64();

void Alloc64_Init();

#endif // _NEKOT1_ALLOC64_H_
