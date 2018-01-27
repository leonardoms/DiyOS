#include <x86/memory.h>
#include <debug/bochs.h>

uint32_t frames_max;
uint32_t kframes[1024];   // FIXME: max of 4MB for kernel
uint32_t iframes[256];    // for low memory identity paging (0..1MB)
extern uint32_t page_directory[1024];

// from Linker script
extern uint32_t kernel_paddr_start;
extern uint32_t kernel_paddr_end;
extern uint32_t kernel_vaddr_start;
extern uint32_t kernel_vaddr_end;

#define VADDRESS_TO_PAGEDIR(addr)   (( (uint32_t)addr >> 12 ) & 0x3FF )
#define VADDRESS_TO_PAGETABLE(addr) ( (uint32_t)addr >> 22 )
#define VADDRES_TO_PHYSICAL(addr)   ( (uint32_t)addr - (uint32_t)&kernel_vaddr_start + (uint32_t)&kernel_paddr_start)
#define BYTES_TO_FRAMES

#define PAGE_FLAG_PRESENT    (1 << 0)
#define PAGE_FLAG_RW         (1 << 1)
#define PAGE_FLAG_USER       (1 << 2)

void
memory_alloc_table(uint32_t virt_addr, uint32_t* pg_table, uint32_t flags);

uint32_t
memory_bytes_to_frames(uint32_t size) {
  uint32_t f;
  f = size / 4096;
  if( size % 4096)
    f++;
  return f;
}

#define PAGE_TABLE_ADDR   0xFFC00000    // page tables at end of linear address
#define PAGE_TABLE_ENTRY  (PAGE_TABLE_ADDR >> 22)
uint32_t   page_table_paddr;
uint32_t*  page_table;

uint32_t   page_table_temp[1024] __attribute__((aligned (4096))); // onde page table in kernel space (with access!)

void
setup_memory(uint32_t mem_size) {
  uint32_t i;

  printf("Paging enabled: kernel @ 0x%x (%dKB)\n", (uint32_t)&kernel_vaddr_start, ((uint32_t)&kernel_vaddr_end-(uint32_t)&kernel_vaddr_start)/1024);

  // alloc one directory for page tables itself
  // get a physical address after kernel (4MB aligned)!

  page_table_paddr = (kernel_paddr_end & 0xFFC00000) + 0x400000;

  // initialize page_table for PAGE_TABLE_ADDR entry
  for(i = 0; i < 1024; i++) {
      page_table_temp[i] = ((uint32_t)page_table_paddr + 0x1000 * i);
      page_table_temp[i] |= PAGE_FLAG_RW | PAGE_FLAG_PRESENT;
  }

  // allow access for page_table_paddr area ...
  memory_alloc_table(PAGE_TABLE_ENTRY,&page_table_temp,PAGE_FLAG_RW | PAGE_FLAG_PRESENT);

  page_table = (uint32_t*)PAGE_TABLE_ADDR;
  printf("page_table @ 0x%x\n", page_table);
BOCHS_BREAKPOINT
  page_table[0x10] = 0xDEADBEEF;
BOCHS_BREAKPOINT


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

// alloc 4KB frame
void
memory_alloc_frame(uint32_t virt_addr, uint32_t phys_addr) {

}

// alloc a page table (4MB) for a Virtual Address (4MB aligned)
void
memory_alloc_table(uint32_t virt_addr, uint32_t* pg_table, uint32_t flags) {
   page_directory[virt_addr >> 22] = VADDRES_TO_PHYSICAL(pg_table) | flags;
}

uint32_t m_bitmap[0];

void
memory_bitmap_init() {

}
