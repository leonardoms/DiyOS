#include <x86/memory.h>
#include <debug/bochs.h>

extern uint32_t page_directory[1024];

#define BYTES_TO_FRAMES //TODO: replace memory_bytes_to_frames(uint32_t size)

#define PAGE_FLAG_PRESENT    (1 << 0)
#define PAGE_FLAG_RW         (1 << 1)
#define PAGE_FLAG_USER       (1 << 2)

#define PAGE_TABLE_SIZE   0x1000
#define PAGE_TABLE_ADDR   0xFFC00000    // page tables at end of linear address
#define PAGE_TABLE_ENTRY  (PAGE_TABLE_ADDR >> 22)

uint32_t   page_table_paddr;
uint32_t*  page_table;
uint32_t   page_table_temp[1024] __attribute__((aligned (4096))); // onde page table in kernel space (with access!)

#define    PHYSICAL_TO_BITMAP(addr)       ( ( (addr >> 22) * 0x80 ) + ((addr >> 12) & 0x3FF000) >> 3)
#define    PHYSICAL_TO_BITMAP_BIT(addr)   ( ( (addr >> 22) * 0x80 ) + ((addr >> 12) & 0x3FF000) % 8)
#define    BITMAP_SIZE     0x20000
uint8_t    m_bitmap[BITMAP_SIZE];

void
memory_set_table(uint32_t virt_addr, uint32_t* pg_table, uint32_t flags);

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
  uint32_t i, j;

  printf("Paging enabled: kernel @ 0x%x (%dKB)\n", (uint32_t)&kernel_vaddr_start, ((uint32_t)&kernel_vaddr_end-(uint32_t)&kernel_vaddr_start)/1024);

  // alloc one directory for page tables itself
  // get a physical address after kernel (4MB aligned)!
  page_table_paddr = (kernel_paddr_end & 0xFFC00000) + 0x400000;

  // initialize page_table_temp for PAGE_TABLE_ADDR entry
  for(i = 0; i < 1024; i++) {
      page_table_temp[i] = ((uint32_t)page_table_paddr + 0x1000 * i);
      page_table_temp[i] |= PAGE_FLAG_RW | PAGE_FLAG_PRESENT;
  }
  // finally allow access for PAGE_TABLE_ADDR area ...
  memory_set_table(PAGE_TABLE_ADDR,&page_table_temp,PAGE_FLAG_RW | PAGE_FLAG_PRESENT);

  // populate the new page tables area!
  page_table = (uint32_t*)PAGE_TABLE_ADDR;

  // set all frames as 'not present'
  for(i = 0; i < 1024; i++)
  for(j = 0; j < 1024; j++)
    page_table[1024*i+j] = 0;

  uint32_t* table_addr;
  for(i = 0; i < 1024; i++) // search for temporary page tables in all page directory
    if(page_directory[i] && PAGE_FLAG_PRESENT) { // find initialized tables
      table_addr = (uint32_t*)PHYSICAL_TO_VIRTUAL((uint32_t*)(page_directory[i] & 0xFFFFF000));
      for(j = 0; j < 1024; j++) {
          if(table_addr[j] && PAGE_FLAG_PRESENT) { // find initialized frames
              //TODO: initialize Bitmap
              page_table[1024*i+j] = table_addr[j]; // copy the content
          }
      }

  }

  // finally set page_directory to new page_table address!
  for(i = 0; i < 1024; i++) {
        page_directory[i] = (uint32_t)(page_table_paddr+PAGE_TABLE_SIZE*i) | (page_directory[i] & 0xFFF);
  }

}

// alloc 4KB frame
void
memory_set_frame(uint32_t virt_addr, uint32_t phys_addr) {

}

// alloc a page table (4MB) for a Virtual Address (4MB aligned)
void
memory_set_table(uint32_t virt_addr, uint32_t* pg_table, uint32_t flags) {
    page_directory[virt_addr >> 22] = VIRTUAL_TO_PHYSICAL(pg_table) | flags;
}
