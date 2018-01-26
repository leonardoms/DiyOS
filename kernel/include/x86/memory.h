#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <ctypes.h>
#include <debug/bochs.h>
#include <small.h>

void
setup_memory(uint32_t mem_size);

void
memory_alloc_table(uint32_t virt_addr, uint32_t* pg_table, uint32_t flags);

#endif
