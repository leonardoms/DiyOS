#include <x86/x86.h>
#include <drivers/fb.h>
#include <drivers/kb.h>
#include <drivers/timer.h>
#include <drivers/bochs_vbe.h>
#include <drivers/rtl81xx.h>
#include <drivers/rtl8139.h>
#include <debug/bochs.h>
#include <debug/assert.h>
#include <debug/test.h>
#include <small.h> // stuffs like printf, str helpers... mem helpers, etc.

extern void* multiboot_info;

extern void art_show();

void do_it_yourself() {
  disable();      // "please, dont disturb". (no interrupts while setting up)

  setup_fb();     // enables built-in video
  printf("DiyOS - do it yourself Operating System\n");
  art_show();

  setup_x86();    // enables x86 32-bits things
  printf("0x%x\n", (uint32_t)(&multiboot_info)[1]);
  setup_memory(0);

  setup_kb();     // enables built-in keyboard
  setup_timer();  // enable timer

  setup_bochs_vbe();

  // network drivers
  setup_ne2000();
  setup_rtl81xx();
  setup_rtl8139();

  test();

  enable();       // crossing fingers... wellcome to the jungle.

  while(1);
}
