#ifndef _X86_H_
#define _X86_H_

#include <ctypes.h>
#include <x86/pic.h>

#define KERNEL_CODE   0x08
#define KERNEL_DATA   0x10
#define USER_CODE     0x18
#define USER_DATA     0x20

uint32_t k_eip;
extern uint32_t kernel_eip();
uint32_t kernel_stack_addr;

typedef struct
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t gs, fs, es, ds;
    uint32_t intn, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} isr_regs_t;

#define irq_regs_t isr_regs_t

typedef void (*isr_callback_t)(isr_regs_t);
typedef void (*irq_callback_t)(isr_regs_t);

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

// "nothing is created, everything is copied"! Thanks OSDev.org
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

void setup_gdt();
void setup_idt();

extern void to_user(uint32_t user_eip);
void to_kernel_stack();


void
syscall_setup();

void
isr_install();

void
irq_install_callback(uint8_t irq, isr_callback_t callback);

void
isr_install_callback(uint8_t irq, isr_callback_t callback);

#define x86() { setup_gdt(); setup_idt(); isr_install(); syscall_setup(); }

#endif
