#ifndef _GFX_H_
#define _GFX_H_

#include <drivers/vga.h>
#include <task.h>

typedef struct color {
  uint8_t r, g, b;
} color_t;

void
gfx_putchar(uint32_t x, uint32_t y, color_t fgcolor, color_t bgcolor, const char c);

uint32_t
gfx_width();

uint32_t
gfx_height() ;

#endif
