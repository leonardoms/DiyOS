#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <ctypes.h>

void*
malloc(uint32_t size);

void
free(void* addr);

int
rand(void);

void
srand(unsigned int seed);

#endif
