#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <ctypes.h>
#include <debug/bochs.h>
#include <small.h>

#define USER_STACK_ADDRESS_BASE  0xBF800000
#define USER_STACK_SIZE          0x1000
#define USER_ADDRESS_BASE        0x01000000
#define KERNEL_ADDRESS_BASE      0xC0000000
#define KERNEL_ADDRESS_TOP       0xFFFFFFFF

#define BYTES_TO_FRAMES //TODO: replace memory_bytes_to_frames(uint32_t size)

#define PAGE_FLAG_BITMAP_END    (0x5 << 9)  // use the 3-bit unused in PageTable entry as flag
#define PAGE_FLAG_BITMAP_START  (0x2 << 9)

#define PAGE_TABLE_SIZE   0x1000
#define PAGE_TABLE_ADDR   0xFFC00000    // page tables at end of linear address
#define PAGE_TABLE_ENTRY  (PAGE_TABLE_ADDR >> 22)

#define PAGE_FLAG_PRESENT    (1 << 0)
#define PAGE_FLAG_RW         (1 << 1)
#define PAGE_FLAG_USER       (1 << 2)

// from Linker script
extern uint32_t kernel_paddr_start;
extern uint32_t kernel_paddr_end;
extern uint32_t kernel_vaddr_start;
extern uint32_t kernel_vaddr_end;

uint32_t* current_page_directory;


#define VIRTUAL_TO_PHYSICAL(addr)   ( (uint32_t)addr - (uint32_t)&kernel_vaddr_start + (uint32_t)&kernel_paddr_start)
#define PHYSICAL_TO_VIRTUAL(addr)   ( (uint32_t)addr + (uint32_t)&kernel_vaddr_start - (uint32_t)&kernel_paddr_start)

void
setup_memory(uint32_t mem_size);

void
memory_set_table(uint32_t* pd, uint32_t virt_addr, uint32_t* pg_table, uint32_t flags);
#define page_table_set memory_set_table

void*
kmalloc_p(uint32_t size);

void*
kmalloc(uint32_t size);

void
kfree(void* addr);
#define free(addr)  kfree(addr)

void
user_test();

#endif
