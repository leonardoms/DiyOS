#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <ctypes.h>

void (*putchar)(const char c);
void (*putpixel)(unsigned x, unsigned y, unsigned c);
void (*clear_screen)(uint8_t c);

#endif
