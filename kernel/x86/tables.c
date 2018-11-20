#include <x86/x86.h>
#include <x86/pic.h>
#include <debug/bochs.h>

extern void load_gdt(struct gdt* addr);
extern void load_tss();
extern uint32_t page_directory[1024];

struct gdt_entry gdt_entries[6];
struct gdt gdtr;

typedef struct tss_entry_struct tss_entry_t;

tss_entry_t tss;

void tss_setup() {
    tss.ss0 = KERNEL_DATA;
    __asm__ __volatile__("movl %%esp, %0;":"=m"(tss.esp0)::"%eax");
    // printf("esp:0x%x\n", tss.esp0); while(1);
    tss.cs = KERNEL_CODE;// | 3;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = KERNEL_DATA;// | 3;
    tss.cr3 = (uint32_t)&page_directory[0];
}

void to_kernel_stack(uint32_t stack) {
    tss.esp0 = stack;
}

uint32_t kernel_get_stack() {
  return tss.esp0;
}

void gdt_entry_setup(struct gdt_entry* gdte, uint32_t base, uint32_t limit,
  uint32_t access, uint32_t flags) {

    gdte->low = base & 0xFFFF;
    gdte->low <<= 16;
    gdte->low |= limit & 0xFFFF;

    gdte->high = ((base >> 16) & 0xFF) | ((access & 0xFF) << 8) |
                 (((limit >> 16) & 0xF) << 16) | ((flags & 0xF) << 20) |
                 (((base >> 24) & 0xFF) << 24);

}

void setup_gdt() {
  tss_setup();

  gdt_entry_setup(&(gdt_entries[0]), 0x00000000, 0x00000000, 0x00, 0x0);  // null descriptor
  gdt_entry_setup(&(gdt_entries[1]), 0x00000000, 0xFFFFFFFF, 0x9A, 0xC);  // kernel code
  gdt_entry_setup(&(gdt_entries[2]), 0x00000000, 0xFFFFFFFF, 0x92, 0xC);  // kernel data
  gdt_entry_setup(&(gdt_entries[3]), 0x00000000, 0xFFFFFFFF, 0xFA, 0xC);  // user code
  gdt_entry_setup(&(gdt_entries[4]), 0x00000000, 0xFFFFFFFF, 0xF2, 0xC);  // user data
  gdt_entry_setup(&(gdt_entries[5]), VIRTUAL_TO_PHYSICAL(&tss), sizeof(tss_entry_t), 0x89, 0xC);

  gdtr.size = 8 * 6;
  gdtr.offset = (uint32_t)&gdt_entries[0];

  load_gdt(&gdtr);
  load_tss();
}

#define USER_STACK 0xBFFFFFFB

static uint32_t  user_code_page[1024] __attribute__((aligned (4096)));
static uint32_t  user_stack_page[1024] __attribute__((aligned (4096)));

// void
// user_setup() {
//   int i;
//   for(i=0;i<1024;i++) {
//     user_code_page[i] = 0;
//     user_stack_page[i] = 0;
//   }
//   // setup user space memory; begins at 0x00000000 (virtual)
//   memory_set_table(0x400000, &user_code_page, PAGE_FLAG_USER );
//   _kmalloc(0x100000,2,766,PAGE_FLAG_USER); // alloc 1MB for user process above 0x100000
//   memory_set_table(0xBF800000-1, &user_stack_page, PAGE_FLAG_USER ); // 4MB bellow kernel virtual memory
//   _kmalloc(0x1000,766,767,PAGE_FLAG_USER); // alloc 4KB for user stack at 0xBFC00000 ~ 0xC0000000 area
//   kmalloc(1);
// }

void
user_test() {
  while(1);
  __asm__ __volatile__("cli\n"); // must generate a GPF
}
