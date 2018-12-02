#ifndef _STDIO_H_
#define _STDIO_H_

#include <ctypes.h>

void
printf(const char *format, ...);

void
outportb(uint32_t port, uint8_t value);

uint8_t
inportb(uint32_t port);

void
outportw(uint32_t port, uint16_t value);

uint16_t
inportw(uint32_t port);

void
outportl(uint32_t port, uint32_t value);

uint32_t
inportl(uint32_t port);

void
enable(void);

void
disable(void);

#endif
