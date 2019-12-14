#include <kernel.h>
#include <drivers/kb.h>
#include <drivers/timer.h>
#include <multiboot.h>



void do_it_yourself(uint32_t multiboot_info) {
  disable();      // "please, dont disturb". (no interrupts while setting up)

  fb();

  multiboot_info_t *mbinfo = (multiboot_info_t *) multiboot_info;
  fs_node_t* ram = NULL;

  printf("DiyOS - do it yourself Operating System\n");

  arch();       // configure arch-dependent things
  mm(mbinfo);   // memory manager

  fs_root();  // initialize FS and /dev
  serial();

  debug("/dev/com1"); // debugging messages to Serial Port 1

  if(mbinfo->mods_count > 0) // has ramdisk.tar module?
    ram = ramfs(((multiboot_module_t*)(mbinfo->mods_addr))[0].mod_start, ((multiboot_module_t*)(mbinfo->mods_addr))[0].mod_end);    // start ramdisk at first module location

#if 1
  struct dirent* d = NULL;
  while( (d = readdir("/", d)) != NULL )
    debug_printf("/%s\n", d->name);
  while( (d = readdir("/ram", d)) != NULL )
    debug_printf("/ram/%s\n", d->name);
  while( (d = readdir("/ram/ui", d)) != NULL )
    debug_printf("/ram/ui/%s\n", d->name);
  while( (d = readdir("/dev", d)) != NULL )
    debug_printf("/dev/%s\n", d->name);
#endif

  tasking();   // multitasking

  // ide();

  gfx();        // graphical server
  gfx_bochs();

  // acpi();   // configure ACPI

  kb();     // enable keyboard
  mouse();  // enable mouse
  timer();  // enable timer
  //
  // // if( !gfx_bochs() )  // load bochs vbe
  // //   gfx_vga(); // if Bochs VBE not present, run 320x200x8 VGA driver
  gui();        // graphical user interface server

  schedule();   // run a kernel thread

  PANIC("end of main loop."); // if something goes wrong...
}
