
#include <drivers/bochs_vbe.h>
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

void bochs_vbe_putpixel_24(uint16_t x, uint16_t y, uint32_t color);
uint16_t bochs_vbe_version();
void bochs_vbe_text_copy_vga_buffer();
void bochs_vbe_text_putchar(const char c);
void bochs_vbe_text_puts(const char* str);

uint8_t bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function;
uint8_t* bochs_vbe_fb;
uint16_t w, h, d;
uint32_t scanline, pixel_size;
uint8_t font_xsize, font_ysize;
uint32_t font_color, font_bgcolor;

// text-mode emulated
char vga_text_buffer[80*(24+1)]; // +1 -> '\n' at end of rows
uint8_t row, col, rows, cols;

//FIXME: testing only (pre-"kmalloc")
uint32_t page_table[2048] __attribute__((aligned (4096)));   // 8MB

void
setup_bochs_vbe() {
    uint32_t aux;

    printf("Bochs VGA: ");
    if(!pci_find(BOCHS_VBE_VENDOR, BOCHS_VBE_DEVICE, &bochs_vbe_bus,
                &bochs_vbe_dev, &bochs_vbe_function)) {
      printf("not found\n");
      return;
    }
    printf("found @ pci/%d:%d.%d\n", bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function);

    if( bochs_vbe_version() < 0xB0C5) {
      printf("VBE version is too old (0x%x)! aborted.\n", bochs_vbe_version());
      return;
    }

    font_xsize = 8;
    font_ysize = 8;
    font_color = 0xF0FAF0;
    font_bgcolor = 0;
    row = col = 0;


    aux = pci_read(bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function, PCI_BAR0);
    aux = aux & 0xF;
    bochs_vbe_fb = (uint8_t*)0xE0000000; // force 0xE0000000 address
    pci_write(bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function, PCI_BAR0, (uint32_t)bochs_vbe_fb | aux);
// pre-kmalloc testing
#if 1
    uint32_t i;
    for(i = 0; i < 2048; i++) {
      page_table[i] = (uint32_t)bochs_vbe_fb + 0x1000 * i;   // put video at same address. BOCHS VBE internal address!
      page_table[i] |= 3;
    }
    // alloc 8MB Video RAM
    memory_alloc_table((uint32_t)bochs_vbe_fb,&page_table[0],0x3);      // first 4MB VRAM
    memory_alloc_table((uint32_t)(bochs_vbe_fb+0x01000000),&page_table[1024],0x3); // last 4MB VRAM.
#endif

    bochs_vbe_text_copy_vga_buffer(); // must be before change video settings
    bochs_vbe_display(640,480,24); // set default resolution 640x480 (24-bits)

    // clear screen
    memset(bochs_vbe_fb, 0, scanline*h);

    bochs_vbe_text_puts(vga_text_buffer);
    bochs_vbe_text_puts("\n");
    bochs_vbe_text_puts("\nsay hello to graphic mode!\n");

    putchar = bochs_vbe_text_putchar;
}

void
bochs_vbe_write(uint16_t index, uint16_t value) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, value);
}

uint16_t
bochs_vbe_read(uint16_t index) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return inw(VBE_DISPI_IOPORT_DATA);
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
    case 24:
      bochs_vbe_putpixel = bochs_vbe_putpixel_24;
      pixel_size = 3; // 24-bit colors
      scanline = width * pixel_size;
      break;
    default:
      printf("bochs_vbe_display: %d-bits depth not suported!\n", depth);
      return;
  }

  w = width;
  h = height;
  d = depth;

  cols = (w / font_xsize) - 1;
  rows = (h / font_ysize) - 1;

  bochs_vbe_disable();
  bochs_vbe_write(VBE_DISPI_INDEX_XRES, width);
  bochs_vbe_write(VBE_DISPI_INDEX_YRES, height);
  bochs_vbe_write(VBE_DISPI_INDEX_BPP, depth);
  bochs_vbe_enable();
}

void
bochs_vbe_putpixel_24(uint16_t x, uint16_t y, uint32_t color) {
  uint32_t offset;
  if( x > w || y > h)
      return;
  offset = y * scanline + x * pixel_size;
  bochs_vbe_fb[offset] = (uint8_t)( color & 0xFF );   // set RR
  *((uint16_t*)&(bochs_vbe_fb[offset+1])) = (uint16_t)(color >> 8); // set BBGG
}

void
bochs_vbe_putchar(uint16_t x, uint16_t y, const char c) {
  uint8_t i, j;
  for(i = 0; i < font_xsize; i++)
    for(j = 0; j < font_ysize; j++) {
      bochs_vbe_putpixel(x+i, y+j, ((font8x8_basic[c & 0x7F][j] >> i ) & 1) ? font_color : font_bgcolor );
    }
}

uint16_t
bochs_vbe_version() {
  return bochs_vbe_read(VBE_DISPI_INDEX_ID);
}

////////////////////////////////////////////////////////////////////////////////
// TEXTMODE  EMULATION

void
bochs_vbe_text_cursor_update() {
  // set emulated text cursor to col-row position
}

void
bochs_vbe_text_roll() {

  memcpy(bochs_vbe_fb+scanline*font_ysize, bochs_vbe_fb, scanline*(h-font_ysize));
  // make last line blank
  memset(bochs_vbe_fb+scanline*(h-font_ysize), 0, scanline*font_ysize);
}

void bochs_vbe_text_newline() {
  row++;
  col = 0;
  if( row > rows) {
    bochs_vbe_text_roll();
    row--;
  }
  bochs_vbe_text_cursor_update();
}

// same of bochs_vbe_putchar but coordinates (row,col) are in textmode cursor emulated
void
bochs_vbe_text_putchar(const char c) {
  uint32_t x, y;

  if(c == '\n') {
    bochs_vbe_text_newline();
    return;
  }

  x = col * font_xsize;
  y = row * font_ysize;
  bochs_vbe_putchar(x,y,c);
  col++;
  if( col > cols ) {
    bochs_vbe_text_newline();
  } else {
    bochs_vbe_text_cursor_update();
  }
}

void
bochs_vbe_text_puts(const char* str) {
  uint32_t i = 0;
  while(str[i])
    bochs_vbe_text_putchar(str[i++]);
}

void
bochs_vbe_text_copy_vga_buffer() {
  uint16_t* vga_fb = (uint16_t*)0xB8000;
  uint32_t offset_max, offset, str_offset;
  uint8_t vga_row, vga_col, vga_rpos, vga_cpos;

  vga_rpos = vga_cpos = 0;
  // find last written char position
  for( vga_row = 0; vga_row < 24; vga_row++)
  for( vga_col = 0; vga_col < 80; vga_col++) {
    if( (vga_fb[80*vga_row+vga_col] & 0xFF) != ' ') {
      vga_rpos = vga_row;
      vga_cpos = vga_col;
    }
  }

  offset_max = vga_rpos * 80 + vga_cpos;
  str_offset = 0;

  for( vga_row = 0; vga_row < 24; vga_row++) {
    for( vga_col = 0; vga_col < 80; vga_col++) {
      offset = 80*vga_row+vga_col;
      if(offset > offset_max) {
        str_offset++;
        vga_text_buffer[str_offset] = '\0';
        return;
      }
      uint8_t c = (uint8_t)vga_fb[offset];
      vga_text_buffer[str_offset] = (c < ' ') ? ' ' : c;
      str_offset++;
    }
  //  vga_text_buffer[offset+vga_row+1] = '\n';
  }
}
