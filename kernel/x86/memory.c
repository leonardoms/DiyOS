
#include <ctypes.h>
#include <debug/bochs.h>


uint32_t frames_max;
uint32_t kframes[1024];   //FIXME: pre-allocating max 4MB for Kernel, must be dynamic later.
extern uint32_t* page_directory;

// from Linker script
#define kernel_physaddr_start 0x100000
extern uint32_t kernel_vaddr_start;
extern uint32_t kernel_vaddr_end;

#define VADDRESS_TO_PAGEDIR(addr)   (( addr >> 12 ) & 0x3FF )
#define VADDRESS_TO_PAGETABLE(addr) ( addr >> 22 )
#define VADDRES_TO_PHYSICAL(addr)   ( addr - &kernel_vaddr_start )
#define BYTES_TO_FRAMES

#define PAGE_FLAG_PRESENT    (1 << 0)
#define PAGE_FLAG_RW         (1 << 1)
#define PAGE_FLAG_USER       (1 << 2)

extern void load_pagedir(uint32_t* pagedir_addr);

uint32_t
memory_bytes_to_frames(uint32_t size) {
  uint32_t f;
  f = size / 4096;
  if( size % 4096)
    f++;
  return f;
}

void
setup_memory() {
  //printf("(vaddr_start,vaddr_end) = (0x%x,0x%x)\n", (uint32_t)&kernel_vaddr_start, (uint32_t)&kernel_vaddr_end);

  uint32_t f;

  // set all page_directory entry 'not present'
  for(f = 0; f < 1024; f++) {
    page_directory[f] = 0;
  }

  // setup 0..1MB as identity paging 1:1 -> virt_addr = phys_addr
  for(f = 0; f < 256; f++) {            // 256 frames = 1MB (1 frame = 4KB)
    kframes[f] = 0; // ( 0 << 22 );     // page dir 0 (first page table)
    kframes[f] |= f << 12;              // current frame (0..4MB)
    kframes[f] |= PAGE_FLAG_PRESENT | PAGE_FLAG_RW;
  }

  // lets setup Kernel area! ;)
  uint32_t kf_max = memory_bytes_to_frames((uint32_t)(&kernel_vaddr_end - &kernel_vaddr_start));
  for(f = 0; f < kf_max; f++) {   //FIXME: fixed max size
      kframes[f] = (kernel_physaddr_start & 0xFFFFF000) | (f << 12) | PAGE_FLAG_PRESENT | PAGE_FLAG_RW;
  }

  page_directory[VADDRESS_TO_PAGEDIR((uint32_t)&kernel_vaddr_start)] = (uint32_t)&kframes;

BOCHS_BREAKPOINT
  load_pagedir(&page_directory);
}
