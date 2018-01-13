#ifndef _FB_H_
#define _FB_H_

#include "ctypes.h"
#include "io.h"

#define FC_BLACK      0x0
#define FC_BLUE       0x1
#define FC_GREEN      0x2
#define FC_CYAN       0x3
#define FC_RED        0x4
#define FC_MAGENTA    0x5
#define FC_BROWN      0x6
#define FC_YELLOW     0xE
#define FC_WHITE      0xF
#define FC_DGRAY      0x8
#define FC_LGRAY      0x7
#define FC_LBLUE      0x9
#define FC_LGREEN     0xA
#define FC_LCYAN      0xB
#define FC_LRED       0xC
#define FC_LMAGENTA   0xD
#define FC_GRAY       FC_DGRAY

#define BC_BLACK      0x0
#define BC_BLUE       0x10
#define BC_GREEN      0x20
#define BC_CYAN       0x30
#define BC_RED        0x40
#define BC_MAGENTA    0x50
#define BC_BROWN      0x60
#define BC_YELLOW     0xE0
#define BC_WHITE      0xF0
#define BC_GRAY       0x70

#define BLINK_TEXT    0x80

void fb_setup();
void fb_clear();
void fb_color(uint8_t color);
void fb_gotoxy(uint32_t x, uint32_t y);
void fb_newline();

void fb_putchar(const char c);
void fb_puts(const char* str);

#define setup_fb fb_setup   // just for fashion

#endif
