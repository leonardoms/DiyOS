
#include <gui.h>

bmp_image_t* wl_image = NULL;

void
wallpaper() {
  wl_image = bmp_image_from_file("ui/wallpaper.bmp");

  if( wl_image == NULL )
    return;

  if(wl_image->bpp != 24) {
    printf("wallpaper(): %d bpp not suported.", wl_image->bpp);
    free(wl_image);
  }
}

void
wallpaper_draw() {
  if(wl_image) {
    gfx_draw_data(wl_image->data, wl_image->width, wl_image->height, 0, 0);
  }
}
