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
#include <multiboot.h>
#include <elf.h>
#include <task.h>
#include <panic.h>
#include <mm.h>

void do_it_yourself(uint32_t multiboot_info) {
  disable();      // "please, dont disturb". (no interrupts while setting up)

  multiboot_info_t *mbinfo = (multiboot_info_t *) multiboot_info;

  fb();     // enables built-in video
  printf("DiyOS - do it yourself Operating System\n");

  x86();      // enables x86 32-bits things
  mm(mbinfo); // memory manager

  acpi();   // configure ACPI
  task();   // multitasking
  kb();     // enable keyboard
  timer();  // enable timer
  terminal();  // create terminal
  gfx();    // graphical server
  gui();    // graphical user interface server

  task_start(); // start multitasking!

  PANIC("end of main loop."); // if something goes wrong...
}
