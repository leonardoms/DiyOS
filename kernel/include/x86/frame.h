#ifndef _FRAME_H_
#define _FRAME_H_

#include <ctypes.h>
#include <mm.h>

#define    FRAME_SIZE      0x1000

void      frame_free(uint32_t addr);
uint32_t  frame_get();
uint32_t  frame_get_blk(uint32_t blk_sz);
uint32_t  frame_free_count();
uint32_t  frame_count();

void
frame_setup(uint32_t mem_size, uint32_t reserved_start, uint32_t reserved_end);

#endif
