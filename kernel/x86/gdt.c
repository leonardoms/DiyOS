#include <kernel.h>

struct gdt_entry gdt_entries[6];
struct gdt gdtr;

void gdt_entry_setup(struct gdt_entry* gdte, uint32_t base, uint32_t limit,
  uint32_t access, uint32_t flags);

void gdt() {
  tss();

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

void gdt_entry_setup(struct gdt_entry* gdte, uint32_t base, uint32_t limit,
  uint32_t access, uint32_t flags) {

    gdte->low = base & 0xFFFF;
    gdte->low <<= 16;
    gdte->low |= limit & 0xFFFF;

    gdte->high = ((base >> 16) & 0xFF) | ((access & 0xFF) << 8) |
                 (((limit >> 16) & 0xF) << 16) | ((flags & 0xF) << 20) |
                 (((base >> 24) & 0xFF) << 24);

}
