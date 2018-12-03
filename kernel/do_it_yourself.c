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

  if(mbinfo->mods_count > 0) // has ramdisk.tar module?
    ram = ramfs(((multiboot_module_t*)(mbinfo->mods_addr))[0].mod_start, ((multiboot_module_t*)(mbinfo->mods_addr))[0].mod_end);    // start ramdisk at first module location

  static uint8_t buff[16], sz;
  fs_node_t* file = finddir_fs(ram, "diyos.txt"); // will be used in Open
  if( file ) {
    if( (sz = read_fs(file, 0, 16, &buff)) > 0 ) {
        // buff[sz] = '\0';
        printf("diyos.txt: sz=%d, content=%s\n", sz, buff);
    }
    // free(file);
  } else printf("diyos.txt: file not found.\n");

  acpi();   // configure ACPI
  task();   // multitasking
  kb();     // enable keyboard
  timer();  // enable timer


  gfx();        // graphical server
  // if( !gfx_bochs() )  // load bochs vbe
  //   gfx_vga(); // if Bochs VBE not present, run 320x200x8 VGA driver
  gfx_bochs();
  gui();        // graphical user interface server

  task_start(); // start multitasking!

  PANIC("end of main loop."); // if something goes wrong...
}
