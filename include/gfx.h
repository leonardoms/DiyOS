#ifndef _GFX_H_
#define _GFX_H_

#include <kernel.h>

typedef struct color {
  uint8_t r, g, b;
} color_t;

#define COLOR_TO_332(c) RGB_TO_332(c.r, c.g, c.b)

void
gfx();

uint8_t
gfx_is_ready();

void
putchar(uint8_t c); // textmode putchar (terminal)

// Virtual Functions assigned by video driver
void      (*gfx_put_pixel)(uint32_t x, uint32_t y, color_t c);
void      (*gfx_rect)(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t c);
void      (*gfx_line)(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t c);
void      (*gfx_putchar)(uint32_t x, uint32_t y, color_t fgcolor, color_t bgcolor, const char c);
uint32_t  (*gfx_width)();
uint32_t  (*gfx_height)();
void      (*gfx_flip)();
#endif
