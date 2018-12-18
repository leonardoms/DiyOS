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

  debug("/dev/com1");

  if(mbinfo->mods_count > 0) // has ramdisk.tar module?
    ram = ramfs(((multiboot_module_t*)(mbinfo->mods_addr))[0].mod_start, ((multiboot_module_t*)(mbinfo->mods_addr))[0].mod_end);    // start ramdisk at first module location

  // static uint8_t buff[128], sz;
  // fs_node_t* file = finddir_fs(fs_root_get(), "/dev/random"); // inside tasks, just use open/read functions
  // if( file ) {
  //   if( (sz = read_fs(file, 0, 128, &buff)) > 0 ) {
  //
  //       printf("[test] /dev/random: sz=%d, data=%d %d %d...\n", sz, buff[0], buff[1], buff[2]);
  //   }
  //   // free(file);
  // } else printf("/dev/random: file not found.\n");

  // ide();

  acpi();   // configure ACPI
  tasking();   // multitasking

  kb();     // enable keyboard
  mouse();  // enable mouse
  timer();  // enable timer

  gfx();        // graphical server
  gfx_bochs();
  //
  // // if( !gfx_bochs() )  // load bochs vbe
  // //   gfx_vga(); // if Bochs VBE not present, run 320x200x8 VGA driver
  gui();        // graphical user interface server

  schedule();   // run a kernel thread

  PANIC("end of main loop."); // if something goes wrong...
}
