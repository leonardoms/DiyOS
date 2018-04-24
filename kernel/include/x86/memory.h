#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <ctypes.h>
#include <debug/bochs.h>
#include <small.h>

// from Linker script
extern uint32_t kernel_paddr_start;
extern uint32_t kernel_paddr_end;
extern uint32_t kernel_vaddr_start;
extern uint32_t kernel_vaddr_end;

#define PAGE_FLAG_PRESENT    (1 << 0)
#define PAGE_FLAG_RW         (1 << 1)
#define PAGE_FLAG_USER       (1 << 2)

#define VIRTUAL_TO_PHYSICAL(addr)   ( (uint32_t)addr - (uint32_t)&kernel_vaddr_start + (uint32_t)&kernel_paddr_start)
#define PHYSICAL_TO_VIRTUAL(addr)   ( (uint32_t)addr + (uint32_t)&kernel_vaddr_start - (uint32_t)&kernel_paddr_start)

void
setup_memory(uint32_t mem_size);

void
memory_set_table(uint32_t virt_addr, uint32_t* pg_table, uint32_t flags);

#define kmalloc(sz) _kmalloc(sz,768,1024)
#define malloc(sz)  _kmalloc(sz,0,768)  //alloc memory in userspace

void
kfree(void* addr);
#define free(addr)  kfree(addr)

void
user_test();

#endif
