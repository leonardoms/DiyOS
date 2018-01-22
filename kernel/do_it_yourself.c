#include <x86/x86.h>
#include <drivers/fb.h>
#include <drivers/kb.h>
#include <drivers/timer.h>
#include <debug/bochs.h>
#include <debug/assert.h>
#include <small.h> // stuffs like printf, str helpers... mem helpers, etc.

void do_it_yourself() {
  disable();      // "please, dont disturb". (no interrupts while setting up)

  setup_x86();    // enables x86 32-bits things
  setup_fb();     // enables built-in video
  setup_kb();     // enables built-in keyboard
//  setup_timer();  // enable timer
  printf("DiyOS - do it yourself Operating System\n");
  art_show();

  setup_bochs_vbe();
  //pci_test();

  enable();       // crossing fingers... wellcome to the jungle.
  while(1);
}
