#ifndef _PANIC_H_
#define _PANIC_H_

#include <ctypes.h>

void  panic(const char *message, const char *file, uint32_t line);
void  panic_assert(const char *file, uint32_t line, const char *desc);

#define PANIC(msg) panic(msg, __FILE__, __LINE__);
#define ASSERT_PANIC(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))

#endif
