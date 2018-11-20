
void
page_map(uint32_t p_addr, uint32_t v_addr) {
  uint32_t  *pd, *table, p_idx, t_idx;

  __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(pd)::"%eax");
  pd = (uint32_t*)( (uint32_t)pd & 0xFFFFF000 );

  t_idx = v_addr >> 22;
  pd[t_idx] = pd[t_idx] | PAGE_FLAG_PRESENT | PAGE_FLAG_RW;

  table = (uint32_t*) (pd[t_idx] & 0xFFFFF000);
  p_idx = ( v_addr >> 12 ) & 0x3FF;
  table[p_idx] = (p_addr & 0xFFFFF000) | PAGE_FLAG_PRESENT | PAGE_FLAG_RW | PAGE_FLAG_BITMAP_END | PAGE_FLAG_BITMAP_START;

}
