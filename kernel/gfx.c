
#include <gfx.h>

uint16_t update_x0, update_x1, update_y0, update_y1;

void
gfx_main() {

  gui_create();

  while(1) {
    //task_disable();
    disable();

    gui_draw();

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

void
gfx_start() {
  if(gfx_task) {
    vga_set_graphic();
    gfx_task->state = TS_READY;
  }
}

void
gfx_setup() {
  gfx_task = task_create((uint32_t)gfx_main, "gfx", TS_BLOCKED);

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
gfx_put_pixel(uint32_t x, uint32_t y, uint8_t R, uint8_t G, uint8_t B) {
    vga_write_pixel(x,y,RGB_TO_332(R,G,B));
    GFX_AREA_GROW(x,y);
}

void
gfx_rect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t R, uint8_t G, uint8_t B) {
    vga_rect(x0,y0,x1,y1,RGB_TO_332(R,G,B));
    GFX_AREA_GROW(x0,y0);
    GFX_AREA_GROW(x1,y1);
}

void
gfx_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t R, uint8_t G, uint8_t B) {
  vga_line(x0,y0,x1,y1,RGB_TO_332(R,G,B));
  GFX_AREA_GROW(x0,y0);
  GFX_AREA_GROW(x1,y1);
}

#define COLOR_TO_332(c) RGB_TO_332(c.r, c.g, c.b)

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
