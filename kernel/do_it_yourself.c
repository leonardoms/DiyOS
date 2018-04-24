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

extern void user_sti();

extern void art_show();

void do_it_yourself(uint32_t multiboot_info) {
  disable();      // "please, dont disturb". (no interrupts while setting up)

  setup_fb();     // enables built-in video
  printf("DiyOS - do it yourself Operating System\n");
  art_show();

  setup_x86();    // enables x86 32-bits things

  multiboot_info_t *mbinfo = (multiboot_info_t *) multiboot_info;
  multiboot_module_t *module = (multiboot_module_t *) mbinfo->mods_addr;
  printf("%s: 0x%x\n", module[0].cmdline, module[0].mod_start);

  setup_memory(0);

  setup_kb();     // enables built-in keyboard
  // setup_timer();  // enable timer

  setup_bochs_vbe();
  syscall_setup();
  // network drivers
  // setup_ne2000();
  // setup_rtl81xx();
  // setup_rtl8139();

  // test();
  user_setup();
  uint32_t* u = (uint32_t*)0x800000;
  uint32_t i;
  elf_sect_t  sect[1];
  if(elf32_get_section_by_name(module[0].mod_start, ".text", &sect))
    printf("%s .text @ 0x%x\n", module[0].cmdline, sect[0].offset);
  else
    printf("failed to find .text\n");

  for(i=0;i<(module[0].mod_end-module[0].mod_start);i++)
    u[i] = ((uint32_t*)(module[0].mod_start+0x1000))[i];

  // user_sti();
  k_eip = kernel_eip();
  BOCHS_BREAKPOINT
  to_user((uint32_t)u); // ELF entrypoint

  enable();       // crossing fingers... wellcome to the jungle.

  while(1);
}
