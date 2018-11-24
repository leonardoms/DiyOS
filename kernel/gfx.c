
#include <gfx.h>
#include <drivers/vga.h> //TODO: driver must set virtual functions on GFX Server on driver load.

uint16_t update_x0, update_x1, update_y0, update_y1;
uint8_t  gfx_loaded = 0;

void
gfx_main() {

  gfx_loaded = 1;

  while(1) {
  disable();

    if( update_x1 > 0 ) { // has any area to re-draw? (test a update_?? coordinate )
      // vga_flip_area(update_x0, update_y0, update_x1, update_y1); // update only modified area
      vga_flip();
      update_x0 = 0xFFFF;
      update_y0 = 0xFFFF;
      update_x1 = 0x0;
      update_y1 = 0x0;
    }
    // task_enable();
    // vga_draw_pointer(); // write mouse pointer directly on video memory
    enable();
    sleep(50); // 20 fps

  }

}

task_t* gfx_task;

uint8_t
gfx_is_ready() {
  return gfx_loaded;
}

void
gfx() {
  vga_set_graphic();

  gfx_task = task_create((uint32_t)gfx_main, "gfx", TS_READY);

  update_x0 = 0xFFFF;
  update_y0 = 0xFFFF;
  update_x1 = 0x0;
  update_y1 = 0x0;

  task_add(gfx_task);
}

#define GFX_AREA_GROW(x,y)  { \
                            if( x > update_x1 ) update_x1 = x; \
                            if( x < update_x0 ) update_x0 = x; \
                            if( y > update_y1 ) update_y1 = y; \
                            if( y < update_y0 ) update_y0 = y; \
                            }

void
gfx_put_pixel(uint32_t x, uint32_t y, color_t c) {
    vga_write_pixel(x,y,COLOR_TO_332(c));
    GFX_AREA_GROW(x,y);
}

void
gfx_rect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t c) {
    vga_rect(x0,y0,x1,y1,COLOR_TO_332(c));
    GFX_AREA_GROW(x0,y0);
    GFX_AREA_GROW(x1,y1);
}

void
gfx_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t c) {
  vga_line(x0,y0,x1,y1,COLOR_TO_332(c));
  GFX_AREA_GROW(x0,y0);
  GFX_AREA_GROW(x1,y1);
}

void
gfx_putchar(uint32_t x, uint32_t y, color_t fgcolor, color_t bgcolor, const char c) {
  vga_putchar(x,y,COLOR_TO_332(fgcolor),COLOR_TO_332(bgcolor),c);
  // GFX_AREA_GROW(x,y);
}

uint32_t
gfx_width() {
  return vga_width();
}

uint32_t
gfx_height() {
  return vga_height();
}
