
#include <gui.h>

bmp_image_t* wl_image = NULL;

void
wallpaper() {
  wl_image = bmp_image_from_file("/ram/ui/wallpaper.bmp");

  if( wl_image == NULL ) {
    return;
  }

  if(wl_image->bpp != 24) {
    printf("wallpaper(): %d bpp not suported.", wl_image->bpp);
    free(wl_image);
  }

  if( (wl_image->width != gfx_width()) || (wl_image->height != gfx_height())) {
    printf("wallpaper image must have %dx%d size.\n", gfx_width(), gfx_height());
  }
}

void
wallpaper_draw_all() {
  if(wl_image) {
    gfx_draw_data(wl_image->data, wl_image->width, wl_image->height, 0, 0);
  }
}

void
wallpaper_draw_area(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  uint32_t p_sz = wl_image->bpp >> 3;
  uint32_t stride = wl_image->width * p_sz;

  if( (x + width) > wl_image->width )
    width = wl_image->width - x;

  if( (y + height) > wl_image->height )
    height = wl_image->height - x;

  uint32_t _y;
  for( _y = y; _y < (y + height); _y++)
    gfx_draw_data(&wl_image->data[ _y * stride + x * p_sz], width, 1, x, _y);
}
