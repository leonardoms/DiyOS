#include <x86/memory.h>

uint32_t frames_max;
uint32_t kframes[1024]; // FIXME: max of 4MB for kernel
uint32_t iframes[256]; // for low memory identity paging (0..1MB)
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

uint32_t
memory_bytes_to_frames(uint32_t size) {
  uint32_t f;
  f = size / 4096;
  if( size % 4096)
    f++;
  return f;
}

void
setup_memory(uint32_t mem_size) {
  printf("Paging enabled: kernel @ 0x%x (%dKB)\n", (uint32_t)&kernel_vaddr_start, ((uint32_t)&kernel_vaddr_end-(uint32_t)&kernel_vaddr_start)/1024);

#if 0
  uint32_t f;
  uint32_t kf_max = memory_bytes_to_frames((uint32_t)(&kernel_vaddr_end - &kernel_vaddr_start));


  // set all page_directory entry 'not present'
  BOCHS_BREAKPOINT
  for(f = 0; f < 1024; f++) {
    page_directory[f] = 0;
  }
  // WARNING: NO FAR JUMPs (eg.: call functions) CAN BE MAKE UNTIL END OF CONFIGURATION

  // setup 0..1MB as identity paging 1:1 -> virt_addr = phys_addr
  BOCHS_BREAKPOINT
  for(f = 0; f < 256; f++) {            // 256 frames = 1MB (1 frame = 4KB)
    iframes[f] = 0; // ( 0 << 22 );     // page dir 0 (first page table)
    iframes[f] |= f << 12;
    iframes[f] |= PAGE_FLAG_PRESENT | PAGE_FLAG_RW;
  }

  // lets setup Kernel area! ;)
  BOCHS_BREAKPOINT
  for(f = 0; f < kf_max; f++) {   //FIXME: fixed max size to 4MB
      kframes[f] = (kernel_physaddr_start & 0xFFFFF000) | (f << 12) | PAGE_FLAG_PRESENT | PAGE_FLAG_RW;
  }

  page_directory[0] = (uint32_t)&iframes;
  page_directory[VADDRESS_TO_PAGEDIR((uint32_t)&kernel_vaddr_start)] = (uint32_t)&kframes;
  BOCHS_BREAKPOINT
#endif
}

void
memory_alloc_virtual(uint32_t virt_addr, uint32_t phys_addr, uint32_t size) {
    
}
