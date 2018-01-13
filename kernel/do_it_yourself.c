#include "x86.h"
#include "fb.h"

void do_it_yourself() {
  fb_setup();
  fb_puts("tester string\n");
  fb_puts("DiyOS - do it yourself Operating System\n");
  fb_puts("Line 2\n");
  fb_puts("Line 3"); fb_newline();
  fb_puts("Line 4\n");
  fb_puts("Line 5\n");
  fb_puts("Line 6\n");
  fb_puts("Line 7\n");
  fb_puts("Line 8\n");
  fb_puts("Line 9\n");
  fb_puts("Line 10\n");
  fb_puts("Line 11\n");
  fb_puts("Line 12\n");
  fb_puts("Line 13\n");
  fb_puts("Line 14\n");
  fb_puts("Line 15\n");
  fb_puts("Line 16\n");
  fb_puts("Line 17\n");
  fb_puts("Line 18\n");
  fb_puts("Line 19\n");
  fb_puts("Line 20\n");
  fb_puts("Line 21\n");
  fb_puts("Line 22\n");
  fb_puts("Line 23\n");
  fb_puts("Line 24\n");
  fb_puts("Line 25\n");
//  fb_puts("Line 26\n");
  setup_gdt();
//  ((unsigned short*)0xB8000)[1] = 'i' | 0x2700;
  setup_idt();
//  ((unsigned short*)0xB8000)[2] = 'y' | 0x2700;
  while(1);
}
