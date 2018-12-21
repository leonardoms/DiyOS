
#include <drivers/gfx/bochs.h>
#include <gfx.h>
#include "font8x8_basic.h"


#define BOCHS_VBE_VENDOR  0x1234
#define BOCHS_VBE_DEVICE  0x1111

#define VBE_DISPI_IOPORT_INDEX  0x1CE
#define VBE_DISPI_IOPORT_DATA   0x1CF

#define VBE_DISPI_INDEX_ID          0
#define VBE_DISPI_INDEX_XRES        1
#define VBE_DISPI_INDEX_YRES        2
#define VBE_DISPI_INDEX_BPP         3
#define VBE_DISPI_INDEX_ENABLE      4
#define VBE_DISPI_INDEX_BANK        5
#define VBE_DISPI_INDEX_VIRT_WIDTH  6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET    8
#define VBE_DISPI_INDEX_Y_OFFSET    9

// flags for VBE_DISPI_INDEX_ENABLE
#define VBE_DISPI_DISABLED          0x00
#define VBE_DISPI_ENABLED           0x01
#define VBE_DISPI_LFB_ENABLED       0x40
#define VBE_DISPI_NOCLEARMEM        0x80

uint8_t bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function;
uint8_t* bochs_vbe_fb;
uint32_t* bochs_vbe_fb_double[2];
uint8_t  bochs_vbe_fb_current = 0, bochs_vbe_fb_pages;
uint16_t w, h, d;
uint32_t scanline, pixel_size;
uint8_t font_xsize, font_ysize;

static uint32_t  update_x1, update_x0, update_y1, update_y0;
// expand ploted area
#define BOCHS_AREA_GROW(x,y)  { \
                            if( x > update_x1 ) update_x1 = x; \
                            if( x < update_x0 ) update_x0 = x; \
                            if( y > update_y1 ) update_y1 = y; \
                            if( y < update_y0 ) update_y0 = y; \
                            }


void
bochs_vbe_putpixel_32(uint32_t x, uint32_t y, color_t c);

void
bochs_vbe_write(uint16_t index, uint16_t value) {
    outportw(VBE_DISPI_IOPORT_INDEX, index);
    outportw(VBE_DISPI_IOPORT_DATA, value);
}

uint16_t
bochs_vbe_read(uint16_t index) {
    outportw(VBE_DISPI_IOPORT_INDEX, index);
    return inportw(VBE_DISPI_IOPORT_DATA);
}

void
bochs_vbe_disable() {
    bochs_vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

void
bochs_vbe_enable() {
    bochs_vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

void
bochs_vbe_display(uint16_t width, uint16_t height, uint16_t depth) {

    switch(depth) {
      case 32:
        // bochs_vbe_putpixel = bochs_vbe_putpixel_24;
        scanline = width * 4;
        pixel_size = 4; // 24-bit colors
        break;
      default:
        printf("bochs_vbe_display: %d-bits depth not suported!\n", depth);
        return;
    }

    w = width;
    h = height;
    d = depth;

    bochs_vbe_disable();
    bochs_vbe_write(VBE_DISPI_INDEX_XRES, width);
    bochs_vbe_write(VBE_DISPI_INDEX_YRES, height);
    bochs_vbe_write(VBE_DISPI_INDEX_BPP, depth);
    bochs_vbe_enable();
    bochs_vbe_write(VBE_DISPI_INDEX_VIRT_WIDTH, width);
    bochs_vbe_write(VBE_DISPI_INDEX_Y_OFFSET, 10);

  }

void
bochs_vbe_putpixel_32(uint32_t x, uint32_t y, color_t c) {
    uint32_t offset;
    if( x > w || y > h)
        return;
    offset = y * w + x;
    bochs_vbe_fb_double[1][offset] = c.value;

    BOCHS_AREA_GROW(x,y);
}

void
bochs_vbe_putchar(uint32_t x, uint32_t y, color_t fgcolor, color_t bgcolor, const char c) {
    uint8_t i, j;
    for(i = 0; i < font_xsize; i++)
      for(j = 0; j < font_ysize; j++) {
        bochs_vbe_putpixel_32( x+i, y+j, ((font8x8_basic[c & 0x7F][j] >> i ) & 1) ? fgcolor : bgcolor );
      }
}

uint16_t
bochs_vbe_version() {
    return bochs_vbe_read(VBE_DISPI_INDEX_ID);
}

uint32_t
bochs_vbe_width() {
  return w;
}

uint32_t
bochs_vbe_height() {
  return h;
}


void
bochs_vbe_rect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t color) {
	uint32_t x, y, temp;

  if(y0 > y1) {
    temp = y0;
    y0 = y1;
    y1 = temp;
  }
  if(x0 > x1) {
    temp = x0;
    x0 = x1;
    x1 = temp;
  }

  for( x = x0; x < x1; x++ )
  for( y = y0; y < y1; y++ )
    bochs_vbe_putpixel_32(x,y,color);
}

void
bochs_vbe_draw_data(uint8_t* data, uint32_t width, uint32_t height, uint32_t x, uint32_t y) {
  uint32_t _y = 0;
  for( ; _y <= height; _y++, y++ ) {
    memcpy(&((uint32_t*)data)[_y * width], &bochs_vbe_fb_double[1][y*w+x], width * 4);
  }
}

void
bochs_vbe_flip() {

  if( update_x1 > 0 ) { // has something to plot ?
    // bochs_vbe_write(VBE_DISPI_INDEX_Y_OFFSET, h * bochs_vbe_fb_current + 10);

    // printf("flip {(%d,%d),(%d,%d)}\n", update_x0, update_y0, update_x1, update_y1);

    uint32_t y = update_y0, offset;
    for( ; y <= update_y1; y++ ) {
      offset = (y * w + update_x0);
      memcpy(&bochs_vbe_fb_double[1][offset], &bochs_vbe_fb_double[0][offset], (update_x1 - update_x0) * 4);
    }

    // bochs_vbe_fb_current = (bochs_vbe_fb_current == 0);
    //
    update_x0 = update_y0 = 0xFFFFFFFF;
    update_x1 = update_y1 = 0x0;
  }

}

uint8_t
gfx_bochs() {
    uint32_t aux, *pagetable;

    printf("Bochs VGA: ");
    if(!pci_find(BOCHS_VBE_VENDOR, BOCHS_VBE_DEVICE, &bochs_vbe_bus,
                &bochs_vbe_dev, &bochs_vbe_function)) {
      printf("not found\n");
      return 0;
    }
    printf("found @ pci/%d:%d.%d\n", bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function);

    // if( bochs_vbe_version() < 0xB0C5) {
    //   printf("\tVBE version is too old (0x%x)! aborted.\n", bochs_vbe_version());
    //   return 0;
    // }

    font_xsize = 8;
    font_ysize = 8;

    aux = pci_read(bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function, PCI_BAR0);

    aux = aux & 0xF;
    bochs_vbe_fb = (uint8_t*)0xE0000000;
    pci_write(bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function, PCI_BAR0, (uint32_t)bochs_vbe_fb | aux);

    uint32_t i;
    for(i = 0; i < 1024; i++) { // 4MB
      page_map(((uint32_t)(bochs_vbe_fb) + 0x1000 * i),((uint32_t)bochs_vbe_fb + 0x1000 * i));
    }

    bochs_vbe_fb = (uint8_t*)0xE0000000 + 10 * 640 * 4 ; // skip 10 lines. 0xE0000000 is unmaped after switch_to (bug?)

    bochs_vbe_fb_current = 0;
    bochs_vbe_fb_double[0] = (uint32_t*)bochs_vbe_fb;
    bochs_vbe_fb_double[1] = (uint32_t*)(bochs_vbe_fb + 640 * 480 * 4);

    update_x0 = update_y0 = 0xFFFFFFFF;
    update_x1 = update_y1 = 0x0;

    gfx_put_pixel = bochs_vbe_putpixel_32;
    gfx_putchar = bochs_vbe_putchar;
    gfx_width = bochs_vbe_width;
    gfx_height = bochs_vbe_height;
    gfx_rect = bochs_vbe_rect;
    gfx_flip = bochs_vbe_flip;
    gfx_draw_data = bochs_vbe_draw_data;

    bochs_vbe_display(640,480,32); // set default resolution 640x480 (32-bits)

    // clear screen
    memset(bochs_vbe_fb, 0x0, 640 * 480 * 4 * 2);

    return 1;
}
