
#include <x86/frame.h>
#include <panic.h>

// aloca memoria no espaço abaixo da área de usuario e kernel de forma 1:1
void*
kmalloc_p(uint32_t size) {
    // static uint32_t p_idx = 0;
    uint32_t p_idx, t_idx, i;
    uint32_t *pd, *table;
    uint32_t page, pages_required, p_addr;
    uint32_t table_limit = USER_ADDRESS_BASE >> 22;

    // pegar um frame
    //  se for na area permitida
    //    mapear 1:1

    pages_required = size / FRAME_SIZE;
    if( (size % FRAME_SIZE) > 0 )
      pages_required++;

    // ASSERT_PANIC(pages_required == 1); // not implemented

    p_addr = frame_get_blk(pages_required);

    if( p_addr == 0 )
      return NULL;

    // bellow USER and KERNEL area
    if( p_addr >= USER_ADDRESS_BASE ) { // THIS IS RLY IMPORTANT!!!
       frame_free(p_addr);
       return NULL;
    }

    __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(pd)::"%eax");

    pd = (uint32_t*)( (uint32_t)pd & 0xFFFFF000 );

    // map the physical address (1:1)
    for( i = 0; i < pages_required; i++ ) {
      t_idx = (p_addr + i * 0x1000) >> 22;
      // if( (pd[t_idx] && PAGE_FLAG_PRESENT) == FALSE )
        pd[t_idx] = pd[t_idx] | PAGE_FLAG_PRESENT | PAGE_FLAG_RW;

      table = (uint32_t*) (pd[t_idx] & 0xFFFFF000);
      p_idx = ( (p_addr + i * 0x1000) >> 12 ) & 0x3FF;
      table[p_idx] = ((p_addr + i * 0x1000) & 0xFFFFF000) | PAGE_FLAG_PRESENT | PAGE_FLAG_RW | PAGE_FLAG_BITMAP_END | PAGE_FLAG_BITMAP_START;
    }
    // printf("dir=0x%x, t_idx=%d, table=0x%x, p_idx=%d, pg_val: 0x%x \n", (uint32_t)pd, t_idx, (uint32_t)table, p_idx, table[p_idx]);
     // printf("kmalloc_p(): 0x%x\n", (uint32_t)p_addr);

    return (void*)p_addr;
}


void*
kmalloc(uint32_t size) {
  uint32_t  *pd, *addr, *page_table;
  uint32_t  frames_left, start_dir, start_page, p_addr, v_addr,
            pages_required, i, j;
  // static uint32_t last_addr = KERNEL_ADDRESS_BASE;

  if(size == 0)
    return NULL;

  pages_required = size / FRAME_SIZE;
  if( (size % FRAME_SIZE) > 0 )
    pages_required++;

  if(pages_required > frame_free_count()) {
    printf("kmalloc: out of memory!\n");
    return NULL;
  }

  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(pd)::"%eax");
  pd = (uint32_t*)( (uint32_t)pd & 0xFFFFF000 );

  // find continguous block of linear memory in high-half 3GB~4GB
  frames_left = pages_required;
  for(i = (KERNEL_ADDRESS_BASE >> 22); i < 1024; i++ ) { // for earch page table
     page_table = (uint32_t*)(pd[i] & 0xFFFFF000);
     // printf("page_table = 0x%x\n", (uint32_t)page_table);
     for(j = 0; j < 1024; j++ ) { // search for free pages
         if((page_table[j] && PAGE_FLAG_PRESENT) == 0) {
             if( frames_left == pages_required ) { // mark first frame
               start_dir = i;
               start_page = j;
             }
             frames_left--;
         } else frames_left = pages_required;

         if( frames_left == 0 )
             break;
     }
     if( frames_left == 0 )
         break;
  }

  if( frames_left > 0 ) {
    printf("kmalloc: out of memory (fragmented)!\n");
    return NULL;
  }

  // initial address
  addr = start_dir * 0x400000 + start_page * 0x1000;

  // map the pages
  for( i = 0; i < pages_required; i++ ) {
    v_addr = (uint32_t)addr + i * 0x1000;
    p_addr = frame_get();

    pd[v_addr >> 22] |= PAGE_FLAG_PRESENT | PAGE_FLAG_RW; // enable table
    page_table = (uint32_t*)(pd[v_addr >> 22] & 0xFFFFF000);
    // maps the page!
    page_table[( v_addr >> 12 ) & 0x3FF] = p_addr | PAGE_FLAG_PRESENT | PAGE_FLAG_RW;

    if( i == 0 )
      page_table[( v_addr >> 12 ) & 0x3FF] |= PAGE_FLAG_BITMAP_START;

    if( (i+1) == pages_required )
      page_table[( v_addr >> 12 ) & 0x3FF] |= PAGE_FLAG_BITMAP_END;

  }

  return addr;
}
