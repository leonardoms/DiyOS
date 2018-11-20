#ifndef _LIB_SMALL_H_
#define _LIB_SMALL_H_

#include <drivers/video.h>
#include <ctypes.h>

/* functions */
void
itoa(uint8_t *buf, uint32_t base, uint32_t d);

uint32_t
strlen(uint8_t* str);

int
strcmp(char *s1, char *s2);

int
strncmp(const char *s1, const char *s2, unsigned int n);

void
memset(void* src, uint8_t value, uint32_t size);

void
memcpy(void* src, void* dst, uint32_t size);

void
printf (const char *format, ...);

uint32_t
getchar();

#endif
