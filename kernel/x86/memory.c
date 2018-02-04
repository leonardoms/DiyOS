#include <x86/memory.h>
#include <debug/bochs.h>

extern uint32_t page_directory[1024];

#define BYTES_TO_FRAMES //TODO: replace memory_bytes_to_frames(uint32_t size)

#define PAGE_FLAG_PRESENT    (1 << 0)
#define PAGE_FLAG_RW         (1 << 1)
#define PAGE_FLAG_USER       (1 << 2)

#define PAGE_FLAG_BITMAP_END    (0x5 << 9)  // use the 3-bit unused in PageTable entry as flag
#define PAGE_FLAG_BITMAP_START  (0x2 << 9)

#define PAGE_TABLE_SIZE   0x1000
#define PAGE_TABLE_ADDR   0xFFC00000    // page tables at end of linear address
#define PAGE_TABLE_ENTRY  (PAGE_TABLE_ADDR >> 22)

uint32_t   page_table_paddr;
uint32_t*  page_table;
uint32_t   page_table_temp[1024] __attribute__((aligned (4096))); // onde page table in kernel space (with access!)
// TODO: use multiboot memory info
#define    memory_frames_count 8192  // 32MB (test)
uint32_t   memory_frames_free;

#define    ADDR_TO_BITMAP(addr)       ( ( (addr >> 22) * 0x80 ) + ((addr >> 12) & 0x3FF000) >> 3)
#define    ADDR_TO_BITMAP_BIT(addr)   ( ( (addr >> 22) * 0x80 ) + ((addr >> 12) & 0x3FF000) % 8)

#define    TABLE_TO_BITMAP(dir,page)  ((dir * 1024) + page) / 8
#define    TABLE_TO_BITMAP_BIT(dir,page)  ((dir * 1024) + page) % 8

#define    BITMAP_SIZE     0x20000
uint8_t    physical_bitmap[BITMAP_SIZE];  // bitmap for physical memory
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

  memory_frames_free = memory_frames_count;
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
              memory_bitmap_set(table_addr[j] * 0xFFFFF000);  // set bitmap as  dirt
              page_table[1024*i+j] = table_addr[j]; // copy the content
          }
      }

  }

  // finally set page_directory to new page_table address!
  for(i = 0; i < 1024; i++) {
        page_directory[i] = (uint32_t)(page_table_paddr+PAGE_TABLE_SIZE*i) | (page_directory[i] & 0xFFF);
  }

  memory_flush();
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

// set memory frame (4KB) as dirty
void
memory_bitmap_set(uint32_t address) {

}

// set memory frame (4KB) as free
void
memory_bitmap_unset(uint32_t address) {

}

void*
kmalloc(uint32_t size) {
   uint32_t frames, frames_left;
   uint32_t*  page_table;
   uint32_t i, j, i_byte, i_bit, addr;
   uint32_t start_dir, start_frame;

   frames = memory_bytes_to_frames(size);

   if( frames > memory_frames_free ) {
     printf("kmalloc: out of memory!\n");
     return NULL;
   }

   frames_left = frames;

   // find continguous block of linear memory in high-half 3GB~4GB
   for(i = 768; i < 1024; i++ ) {
        for(j = 0; j < 1024; j++ ) { // search frames
            page_table = (uint32_t*)(PAGE_TABLE_ADDR + i * 0x1000);
            if((page_table[j] && PAGE_FLAG_PRESENT) == 0) {
                if( frames_left == frames ) { // mark first frame
                  start_dir = i;
                  start_frame = j;
                }
                frames_left--;
            } else frames_left = frames;

            if( frames_left == 0 )
                goto _alloc_frames;
        }
   }

   printf("kmalloc: out of memory (fragmented)!\n");
   return NULL;

_alloc_frames:

   // printf("kmalloc: %d frames @ 0x%x\n", frames, start_dir * 0x400000 + start_frame * 0x1000);

   frames_left = frames;

   // now the reference is the physical memory
   for(i = 2304; i < memory_frames_count; i ++) { // skip 9MB ( 1MB mem low; 4MB kernel; 4MB page table. )
     i_byte = i / 8;
     i_bit = i % 8;
     // printf("#%d: %d\n", i_byte, (uint8_t)physical_bitmap[i_byte] );
     // printf("%d.%d ", i_byte, i_bit );

     if(physical_bitmap[i_byte] && ( 1 << i_bit) ) { // is it a free bit?
        continue; // dirty bit
     }


     physical_bitmap[i_byte] |= ( 1 << i_bit);  // mark the bit as dirty

     addr = (start_dir * 0x400000 + start_frame * 0x1000) + (frames_left - 1) * 0x1000;

     page_table = (uint32_t*)( PAGE_TABLE_ADDR + (addr >> 22) * 0x1000 );

     if( !(page_directory[addr >> 22] && PAGE_FLAG_PRESENT) ) // if is a free directory set as Supervisor; R/W; Present.
        page_directory[addr >> 22] |= PAGE_FLAG_RW | PAGE_FLAG_PRESENT;

     // finally the frames!
     page_table[(addr >> 12) & 0x3FF] = (( i_byte * 8 + i_bit ) * 0x1000 ) | PAGE_FLAG_RW | PAGE_FLAG_PRESENT;  // * 0x200

     if( frames_left == frames )
        page_table[(addr >> 12) & 0x3FF] |= PAGE_FLAG_BITMAP_END; // last frame signature, used in kfree


     //memory_flush(addr); // force reload in TLB when requested

     // printf("\ttable 0x%x[%d] [0x%x], vaddr 0x%x, paddr 0x%x\n", (uint32_t)page_table, (addr >> 12) & 0x3FF, page_table[(addr >> 12) & 0x3FF], (uint32_t)addr, (( i_byte * 8 + i_bit ) * 0x200 ) );

     frames_left--;
     if( frames_left == 0 ) {
       page_table[(addr >> 12) & 0x3FF] |= PAGE_FLAG_BITMAP_START; // first frame signature, used in kfree
       break;
     }

   }

   memory_frames_free -= frames;

   memory_flush();
   BOCHS_BREAKPOINT

   return (void *)addr;
}

void
memory_debug_addr(uint32_t addr) {
    uint32_t* page_table;
    uint32_t  frame_base, i;

    printf("Virtual Address Debug [ 0x%x ]: ", addr);

    page_table = (uint32_t*)( PAGE_TABLE_ADDR + ((addr >> 22) * 0x1000) );
    frame_base = ((addr >> 12) & 0x3FF);

    if( !(page_table[frame_base] && PAGE_FLAG_BITMAP_START) ) {
        printf("invalid! (no start flag)\n");
        return;
    }


    i = 0;
    do {
        printf("\n\tframe#%d virt:0x%x phys:0x%x start:%c end:%c present:%c rw:%c", i, addr + i * 0x1000,
                page_table[frame_base+i] & 0xFFFFF000,
                (page_table[frame_base+i] & PAGE_FLAG_BITMAP_START) ? 'y' : 'n',
                (page_table[frame_base+i] & PAGE_FLAG_BITMAP_END) ? 'y' : 'n',
                (page_table[frame_base+i] & PAGE_FLAG_PRESENT) ? 'y' : 'n',
                (page_table[frame_base+i] & PAGE_FLAG_RW) ? 'y' : 'n'
                );
        // printf("\n\tframe#%d 0x%x\n", i, page_table[frame_base+i]);
    } while ((page_table[frame_base+i++] & PAGE_FLAG_BITMAP_END) == 0);

    printf("\nend of blocks (%d frames)\n", i);

}

void
memory_flush_all() {
   __asm__ __volatile__ ("movl	%cr3,%eax\n");
   __asm__ __volatile__ ("movl	%eax,%cr3\n");
}

void
memory_flush(uint32_t addr) {
   __asm__ __volatile__("invlpg (%0)" ::"r" (addr) : "memory");
}
