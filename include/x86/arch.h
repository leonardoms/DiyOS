#ifndef _ARCH_X86_H_
#define _ARCH_X86_H_

#include <ctypes.h>
#include <x86/page.h>

#define KERNEL_CODE   0x08
#define KERNEL_DATA   0x10
#define USER_CODE     0x18
#define USER_DATA     0x20

uint32_t kernel_stack_addr;

#pragma pack(1)
typedef struct {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t gs, fs, es, ds;
    uint32_t intn, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} int_regs_t;

typedef struct{
	uint32_t ebx, esi, edi, ebp;  // callee-save
	uint32_t eip, eflags;	        // CALL/IRET
} kregs_t;

typedef int (*isr_call_t)(int_regs_t*);
typedef isr_call_t isr_call_vector_t[256];

#pragma pack(1)
typedef struct {
    unsigned    gate;
    isr_call_t  isr_call;
} isr_vector_t;

struct gdt {
  uint16_t size;
  uint32_t offset;
} __attribute__ ((__packed__));

struct idt {
  uint16_t size;
  uint32_t offset;
} __attribute__ ((__packed__));

struct gdt_entry {
    uint32_t low;
    uint32_t high;
} __attribute__ ((aligned (8)));

typedef struct idt_entry {
    uint16_t  offset_1;   // offset bits 0..15
    uint16_t  selector;   // a code segment selector in GDT or LDT
    uint8_t   ist;        // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t   type_attr;  // type and attributes
    uint16_t  offset_2;   // offset bits 16..31
} __attribute__ ((aligned (8), __packed__)) idt_entry_t;


struct tss_entry_struct
{
   uint32_t prev_tss;
   uint32_t esp0;
   uint32_t ss0;
   uint32_t esp1;
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;
   uint32_t cs;
   uint32_t ss;
   uint32_t ds;
   uint32_t fs;
   uint32_t gs;
   uint32_t ldt;
   uint16_t trap;
   uint16_t iomap_base;
} __attribute__ ((__packed__));

typedef struct tss_entry_struct tss_t;
typedef struct tss_entry_struct tss_entry_t;

tss_entry_t kernel_tss;

void gdt();
void idt();

void
syscall_setup();

void
isr_init(void);

void
isr_set_handler(int num, isr_vector_t *isr_vector);

void
isr_get_handler(int num, isr_vector_t *isr_vector);

#define IRQ_SET_HANDLER(irq,handler)  { ASSERT_PANIC(irq <= 0xF); isr_vector_t v = { 0x8E, handler }; isr_set_handler(0x20 + irq,&v); }
#define ISR_SET_HANDLER(n,handler)  { isr_vector_t v = { 0x8E, handler }; isr_set_handler(n,&v); }


#define arch() { gdt(); idt(); isr_install(); }

#endif
