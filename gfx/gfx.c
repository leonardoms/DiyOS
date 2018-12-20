
#include <gfx.h>

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
fs_node_t* lfb;

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

// slow generic version
void
_gfx_draw_data(uint8_t* data, uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
  uint32_t i, j;
  for( j = 0; j < height; j++)
  for( i = 0; i < width; i++ )
    if( (data[(j*width+i)*3] != 255) && (data[(j*width+i)*3+2] != 255) )
      gfx_put_pixel(x+i,y+j, (color_t){data[(j*width+i)*3],data[(j*width+i)*3+1],data[(j*width+i)*3+2]});
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

  // gfx_task = task_create((uint32_t)gfx_main, "gfx", TS_READY);

  // lfb = open("/dev/lfb", 0, 0);
  //
  // uint8_t buf[640*10*3];
  //
  // memset(buf, 0xFF, 640*10*3);
  //
  // if( !lfb ) {
  //   debug_printf("[GFX] could not open /dev/lfb. (aborted)\n");
  //   return;
  // }
  //
  // write(lfb,&buf,640*10*3);

  // dummy functions
  gfx_put_pixel = _gfx_put_pixel;
  gfx_putchar = _gfx_putchar;
  gfx_line = _gfx_line;
  gfx_rect = _gfx_rect;
  gfx_width = _gfx_width;
  gfx_height = _gfx_height;
  gfx_flip = _gfx_flip;
  gfx_draw_data = _gfx_draw_data;

  gfx_loaded = 1;
  // term = terminal();

  // task_add(gfx_task);
}
