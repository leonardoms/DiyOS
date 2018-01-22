#ifndef _LIB_SMALL_H_
#define _LIB_SMALL_H_

#include <drivers/fb.h>
#include <ctypes.h>

/* functions */
void
itoa(uint8_t *buf, uint32_t base, uint32_t d);

uint32_t
strlen(uint8_t* str);

void
printf (const char *format, ...);

#endif
