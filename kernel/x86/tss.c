
#include <arch.h>

void tss() {
    kernel_tss.ss0 = KERNEL_DATA;
    __asm__ __volatile__("movl %%esp, %0;":"=m"(kernel_tss.esp0)::"%eax");
    // kernel_tss.esp0 = (uint32_t)malloc(0x1000) + 0x1000 - 4;
    // __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(kernel_tss.cr3)::"%eax");
    // printf("esp:0x%x\n", kernel_tss.esp0); while(1);
    kernel_tss.cs = KERNEL_CODE;// | 3;
    kernel_tss.ss = kernel_tss.ds = kernel_tss.es = kernel_tss.fs = kernel_tss.gs = KERNEL_DATA;// | 3;

    kernel_tss.iomap_base = 104; //  http://forum.osdev.org/viewtopic.php?t=13678
}

void
tss_set_esp0(uint32_t esp0) {
  kernel_tss.esp0 = esp0;
}
