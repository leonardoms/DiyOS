#ifndef _FB_H_
#define _FB_H_

#include "ctypes.h"

void fb_setup();
void fb_clear();
void fb_color(uint8_t color);
void fb_gotoxy(uint32_t x, uint32_t y);
void fb_newline();

void fb_putchar(const char c);
void fb_puts(const char* str);

#endif
