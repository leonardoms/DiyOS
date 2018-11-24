#include <mm.h>

//TODO: create tool for merge neighbor holes

uint32_t  malloc_top, heap;
mblock_t  *root_mblock, *last_block;

uint32_t
expand(uint32_t requested_size) {
  return page_map_until(malloc_top + requested_size);
}

uint32_t
constrict() {
  mblock_t* last_used;
  mblock_t* b;

  b = root_mblock;

  while(b) {
    if( b->magic == MBLOCK_MAGIC ) {
      if(b->hole == 0) { // is it a used block?
          last_used = b;
      }
    }
    b = b->next;
  }

  return page_free_above(last_used->addr + last_used->size);
}

mblock_t*
_split_hole(mblock_t* b, uint32_t size) {
  mblock_t* new_hole;

  if(!b->hole)
    return NULL;

  if( (b->size - (size + sizeof(mblock_t)) ) < (sizeof(mblock_t) + 4) ) { // remain size is too small ?
    b->hole = 0;
    return b; // dont split, use entire hole!
  } else {
    // create a new hole
    new_hole = (mblock_t*)(b->addr + size);
    new_hole->magic = MBLOCK_MAGIC;
    new_hole->hole = 1;
    new_hole->size = b->size - sizeof(mblock_t) - size;
    new_hole->addr = (b->addr + size + sizeof(mblock_t));
    // old hole now is a used memory.
    b->hole = 0;
    b->size = size;

    new_hole->next = b->next;
    b->next = new_hole;

    return b;
  }
}

mblock_t*
_find_hole(uint32_t size) {
  mblock_t* b;

  b = root_mblock;

  while(b) {
    if( b->magic == MBLOCK_MAGIC ) {
      if(b->hole) { // is it a hole?
        if(b->size >= size) // size fits on this hole?
          return _split_hole(b,size);
      }
    } // else {
    //  printf("\nmblock: wrong magic!\n", );
    // mblock_fix();    // fix memory manager!
    // b = root_mblock; // restart
    // }
    b = b->next;
  }

  return NULL;
}

void*
malloc(uint32_t size) {
  mblock_t* b;

  if(size == 0)
    return NULL;
printf("1");
  b = _find_hole(size); // try to find a hole that fits requested size
  if(b)
    return (void*)b->addr;
printf("2");

  if( (malloc_top + size + sizeof(mblock_t)) > heap ) {
    uint32_t new_heap;
printf("3");
    new_heap = expand(size); // need more memory
printf("4");
    if(new_heap == heap)
      return NULL; // failed to expand (out of memory)
printf("5");
    heap = new_heap;
  }

  b = (mblock_t*)malloc_top;

  b->magic = MBLOCK_MAGIC;
  b->size = size;
  b->addr = malloc_top + sizeof(mblock_t);
  b->next = NULL;

  last_block->next = b;
  last_block = b;

  malloc_top = (uint32_t)(b->addr + b->size);

  return b;
}

void
free(void* address) {
  mblock_t* b = (mblock_t*)((uint32_t)address - sizeof(mblock_t));

  if( b->magic == MBLOCK_MAGIC ) {
    b->hole = 1;
  }

  heap = constrict(); // try to reduce the heap

}

void
mm(multiboot_info_t* mb) {

  multiboot_module_t* mod;
  uint32_t i = 0, heap = 0, ld_kernel_limit;

  ld_kernel_limit = (uint32_t)&kernel_vaddr_end;

  if(ld_kernel_limit == 0)
    PANIC("ld script's invalid Kernel limit");

  heap = ld_kernel_limit;

  // find the heap
  mod = (multiboot_module_t *) mb->mods_addr;
  for(i = 0; i < mb->mods_count; i++){
      if( (KERNEL_BASE + mod->mod_end - 0x100000) > heap ) // mod is in physical memory
        heap = KERNEL_BASE + mod->mod_end - 0x100000;
  }

  frame_setup(mb->mem_upper * 1024, KERNEL_BASE, heap); // start frame manager with original kernel memory as reserved area

  malloc_top = heap;
  heap = (heap & 0xFFFFF000) + 0x1000;
}
