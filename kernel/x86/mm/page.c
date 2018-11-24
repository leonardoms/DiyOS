#include <mm.h>
#include <x86/frame.h>

void
page_map(uint32_t p_addr, uint32_t v_addr) {
  uint32_t  *pd, *table, p_idx, t_idx;

  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(pd)::"%eax");
  pd = (uint32_t*)( (uint32_t)pd & 0xFFFFF000 );

  t_idx = v_addr >> 22;
  pd[t_idx] = pd[t_idx] | PAGE_FLAG_PRESENT | PAGE_FLAG_RW; // map the table

  table = (uint32_t*) (pd[t_idx] & 0xFFFFF000);
  p_idx = ( v_addr >> 12 ) & 0x3FF;
  table[p_idx] = (p_addr & 0xFFFFF000) | PAGE_FLAG_PRESENT | PAGE_FLAG_RW | PAGE_FLAG_BITMAP_END | PAGE_FLAG_BITMAP_START;

}

void
page_unmap(uint32_t v_addr) {
  uint32_t  *pd, *table, p_idx, t_idx;

  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(pd)::"%eax");
  pd = (uint32_t*)( (uint32_t)pd & 0xFFFFF000 );

  t_idx = v_addr >> 22;
  pd[t_idx] = pd[t_idx] | PAGE_FLAG_PRESENT | PAGE_FLAG_RW;

  table = (uint32_t*) (pd[t_idx] & 0xFFFFF000);
  p_idx = ( v_addr >> 12 ) & 0x3FF;

  frame_free(table[p_idx] & 0xFFFFF000);
  table[p_idx] = 0;

  //TODO: check for free the Table entry

}

// get the upper page mapped
uint32_t
page_upper() {
  uint32_t  *pd, *table, p_idx, t_idx, v_addr;

  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(pd)::"%eax");
  pd = (uint32_t*)( (uint32_t)pd & 0xFFFFF000 );

  v_addr = KERNEL_BASE;

  while( v_addr < KERNEL_LIMIT ) {

      t_idx = v_addr >> 22;
      pd[t_idx] = pd[t_idx] | PAGE_FLAG_PRESENT | PAGE_FLAG_RW;

      table = (uint32_t*) (pd[t_idx] & 0xFFFFF000);
      p_idx = ( v_addr >> 12 ) & 0x3FF;

      if( table[p_idx] && PAGE_FLAG_PRESENT )
        continue;
      else
        break;

      v_addr += 0x1000;
  }

  return (v_addr - 0x1000);
}

uint32_t
page_map_until(uint32_t new_limit) {
  uint32_t old_limit, p_addr;

  old_limit = page_upper();

  new_limit &= 0xFFFFF000;
  while( old_limit <= new_limit ) {
    p_addr = frame_get();

    if( !p_addr ) {
      //TODO: hollback the mapped pages
      return old_limit;
    }

    page_map(p_addr,old_limit);

    old_limit += 0x1000;
  }

  return new_limit;
}

uint32_t
page_free_above(uint32_t limit) {
  uint32_t new_page_limit, page_limit;

  new_page_limit = (limit & 0xFFFFF000) + 0x1000;
  page_limit = page_upper();

  while( new_page_limit <= page_limit ) {
    page_unmap(new_page_limit);
    new_page_limit += 0x1000;
  }

}
