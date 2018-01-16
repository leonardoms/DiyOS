#include "x86.h"
#include "pic.h"
#include "bochs.h"

extern void load_gdt(struct gdt* addr);
extern void load_idt(struct idt* addr);

void gdt_entry_setup(struct gdt_entry* gdte, uint32_t base, uint32_t limit,
  uint32_t access, uint32_t flags) {

    gdte->low = base & 0xFFFF;
    gdte->low <<= 16;
    gdte->low |= limit & 0xFFFF;

    gdte->high = ((base >> 16) & 0xFF) | ((access & 0xFF) << 8) |
                 (((limit >> 16) & 0xF) << 16) | ((flags & 0xF) << 20) |
                 (((base >> 24) & 0xFF) << 24);

}

struct gdt_entry gdt_entries[3];
struct gdt gdtr;

void setup_gdt() {
  gdt_entry_setup(&(gdt_entries[0]), 0x00000000, 0x00000000, 0x00, 0x0);  // null descriptor
  gdt_entry_setup(&(gdt_entries[1]), 0x00000000, 0xFFFFFFFF, 0x9A, 0xC);  // kernel code
  gdt_entry_setup(&(gdt_entries[2]), 0x00000000, 0xFFFFFFFF, 0x92, 0xC);  // kernel data

  gdtr.size = 8 * 3;
  gdtr.offset = (uint32_t)&gdt_entries[0];

  load_gdt(&gdtr);
}

struct idt_entry idt_entries[256];
struct idt idtr;

uint32_t callbacks[256];

void idt_entry_setup(uint8_t idx, uint32_t callback) {
    struct idt_entry* e = &idt_entries[idx];
    e->offset_1 = callback & 0xFFFF;
    e->selector = 0x08;
    e->ist = 0x00;
    e->type_attr = 0x8E; // present, 32-bits interrupt, DPL = 0
    e->offset_2 = (callback >> 16) & 0xFFFF;
}

void setup_idt() {
  setup_pic();
  // make sure that all idt entries are zeroed
  uint32_t* e;
  e = (uint32_t*)idt_entries;
  uint32_t i;
  for(i = 0; i < 512; i++)
      e[i] = 0x00000000;

  idtr.size = 8 * 256;
  idtr.offset = (uint32_t)&idt_entries[0];

  load_idt(&idtr);
}

irq_callback_t cb;

//INTERRUPT_CALLBACK
void irq_handle() {

  cb = (irq_callback_t)callbacks[0x21]; // just for testing

  if(cb)
    cb();

  pic_acknowledge(0x21); // test keyboard

  __asm__ __volatile__ ("add $12, %esp\n");
  __asm__ __volatile__("iret\n");
}

void irq_install(uint8_t irq, irq_callback_t callback) {
  if((irq - IRQ_BASE) > 0xF) return;
  idt_entry_setup(irq, (uint32_t)irq_handle);
  callbacks[irq] = (uint32_t)callback;
  irq_enable(irq);
}
