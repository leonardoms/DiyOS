#include <gui.h>

bmp_image_t* iconset_image;
uint32_t iw, ih;

void
iconset(uint8_t* file, uint32_t icon_w, uint32_t icon_h) {
  iconset_image = bmp_image_from_file(file);

  if( iconset_image == NULL ) {
    return;
  }

  if(iconset_image->bpp != 32) {
    debug_printf("iconset(): %d bpp not suported.", iconset_image->bpp);
    free(iconset_image);
  }

  if( (iconset_image->width % icon_w) || (iconset_image->height % icon_h)){
    debug_printf("iconset(): wrong icon size. (%dx%d)", icon_w, icon_h);
    free(iconset_image);
  }

  iw = icon_w;
  ih = icon_h;
}

void
iconset_draw(uint32_t row, uint32_t col, uint32_t x, uint32_t y) {
  uint32_t _y;

  if( ((col + 1) * iw) > iconset_image->width || ((row + 1) * ih) > iconset_image->height )
    return;

  for( _y = 0; _y < ih; _y++ )
    gfx_draw_data_with_alfa(&((uint32_t*)iconset_image->data)[(_y + row * ih)*iconset_image->width+col*iw], iw, 1, x, y + _y);
}
