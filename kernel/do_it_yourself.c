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
#include <x86/memory.h>

extern void user_sti();

extern void art_show();

void do_it_yourself(uint32_t multiboot_info) {
  disable();      // "please, dont disturb". (no interrupts while setting up)

  multiboot_info_t *mbinfo = (multiboot_info_t *) multiboot_info;

  setup_fb();     // enables built-in video
  printf("DiyOS - do it yourself Operating System\n");

  setup_x86();    // enables x86 32-bits things


  // TODO: use multiboot_mmap_entry
  setup_memory(mbinfo->mem_upper * 1024); // in bytes

  // setup_bochs_vbe();
  // vga_set_graphic();
  // network drivers
  // setup_ne2000();
  // setup_rtl81xx();
  // setup_rtl8139();
  acpi_setup();
  task_setup();

  uint32_t i = 0;
  multiboot_module_t *module = (multiboot_module_t *) mbinfo->mods_addr;
  for(i = 0; i < mbinfo->mods_count; i++){
    // if(!initrd_create((uint8_t*)module[i].mod_start, (uint8_t*)module[i].mod_end))
      // task_add(task_create_from_elf((char*)module[i].cmdline, (uint8_t*)module[i].mod_start));
      // module_load((char*)module[i].cmdline, (uint8_t*)module[i].mod_start);
  }

  setup_kb();     // enables built-in keyboard
  setup_timer();  // enable timer

  // terminal_root_setup();

  gfx_setup();
  gfx_start();

  task_start();

  enable();       // crossing fingers... wellcome to the jungle.

  // while(1);
}
