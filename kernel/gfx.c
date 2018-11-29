
#include <gfx.h>
#include <drivers/vga.h> //TODO: driver must set virtual functions on GFX Server on driver load.

// typedef struct gfx_size {
//   uint8_t*  description;
//   uint32_t  w, h, d;
// } gfx_size_t;
//
// typedef struct gfx {
//   uint8_t*    provider_name;
//   gfx_size_t* resolutions;
//
// } gfx_t;

uint8_t  gfx_loaded = 0;

void
gfx_main() {
  printf("GFX started.\n");

  gfx_loaded = 1;

  while(1) {
    disable();

    gfx_flip();

    enable();
    sleep(50); // 20 frames/s
  }

}

task_t* gfx_task;

uint8_t
gfx_is_ready() {
  return gfx_loaded;
}

void
_gfx_put_pixel(uint32_t x, uint32_t y, color_t c) {
    // GFX_AREA_GROW(x,y);
}

//TODO: draw rect/line/char...etc with gfx_put_pixel (not optimized), just in case of the driver not assign primitive funtions!

void
_gfx_rect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t c) {
    // GFX_AREA_GROW(x0,y0);
    // GFX_AREA_GROW(x1,y1);
}

void
_gfx_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t c) {
  // GFX_AREA_GROW(x0,y0);
  // GFX_AREA_GROW(x1,y1);
}

void
_gfx_putchar(uint32_t x, uint32_t y, color_t fgcolor, color_t bgcolor, const char c) {
  // GFX_AREA_GROW(x,y);
}

uint32_t
_gfx_width() {
  return 0;
}

uint32_t
_gfx_height() {
  return 0;
}

void
_gfx_flip() {

}

void
gfx() {

  gfx_task = task_create((uint32_t)gfx_main, "gfx", TS_READY);

  // dummy functions
  gfx_put_pixel = _gfx_put_pixel;
  gfx_putchar = _gfx_putchar;
  gfx_line = _gfx_line;
  gfx_rect = _gfx_rect;
  gfx_width = _gfx_width;
  gfx_height = _gfx_height;
  gfx_flip = _gfx_flip;

  task_add(gfx_task);
}
