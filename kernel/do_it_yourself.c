#include "x86.h"
#include "fb.h"

void do_it_yourself() {
  setup_gdt();
  setup_idt();

  fb_setup();
  fb_puts("DiyOS - do it yourself Operating System\n");
  while(1);
}
