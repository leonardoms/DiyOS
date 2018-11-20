// video built-in
#include <drivers/fb.h>

uint8_t*  fbmem;
uint32_t  x, y, line_size, width, height, cell_size, buff_size;
uint32_t  offset;
uint8_t   color;

void fb_roll();
void fb_update_offset();
void fb_cursor_enable();
void fb_cursor_update();

void fb_setup() {
  fbmem = (uint8_t*)0xB8000;
  x = y = 0;
  offset = 0;
  width = 80;
  height = 24;
  cell_size = 2;                  // 2-bytes long
  line_size = width * cell_size;
  buff_size = line_size * height;
  color = FC_LGRAY | BC_BLACK;
  fb_cursor_enable();   // make sure the cursor is enabled
  fb_clear();
  putchar = fb_putchar; // from video.h
}

void fb_gotoxy(uint32_t _x, uint32_t _y) {
  if( x < width )
    x = _x;
  if( y < height )
    y = _y;

  // offset = y * line_size + x * cell_size;
  fb_update_offset();
  fb_cursor_update();
}

void fb_color(uint8_t _color) {
  color = _color;
}

void fb_clear() {
  offset = 0;
  do {
    fbmem[offset] = ' ';
    fbmem[offset+1] = color;
    offset += 2;
  } while(offset < buff_size);
  fb_gotoxy(0,0);
}

void fb_newline() {
  y++;
  x = 0;
  if( y > height) {
    fb_roll();
    y--;
  }
  fb_update_offset();
  fb_cursor_update();
}

void fb_roll() {
  uint32_t i = 0, j;
  for(; i < height; i++ )
    for(j = 0; j < line_size; j+=2) {
      fbmem[i*line_size+j] = fbmem[(i+1)*line_size+j];      // copy the char
      fbmem[i*line_size+j+1] = fbmem[(i+1)*line_size+j+1];  // copy the color
    }
  // make last line blank
  for(j = 0; j < line_size; j+=2) {
    fbmem[(height)*line_size+j] = ' ';
    fbmem[(height)*line_size+j+1] = color;
  }
}

void fb_putchar(const char c) {
  if(c == '\n') {
    fb_newline();
    return;
  }

  if(c == '\t') {
    fb_puts("   ");
    return;
  }

  fbmem[offset] = c;
  fbmem[offset+1] = color;
  x++;

  if( x > width ) {
    fb_newline();
  } else {
    fb_update_offset();
    fb_cursor_update(x,y);
  }
}

void fb_puts(const char* str) {
  uint32_t i = 0;
  while(str[i])
    fb_putchar(str[i++]);
}


void fb_cursor_enable()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0));

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3E0) & 0xE0) | 15); // 15 = scanline size of cursor
}

void fb_cursor_update() {
  uint16_t pos = offset / cell_size;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void fb_update_offset() {
  offset = y * line_size + x * cell_size;
}
