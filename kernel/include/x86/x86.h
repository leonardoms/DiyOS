#ifndef _X86_H_
#define _X86_H_

#include <ctypes.h>
#include <x86/memory.h>
#include <x86/pic.h>

#define KERNEL_CODE   0x08
#define KERNEL_DATA   0x10
#define USER_CODE     0x18
#define USER_DATA     0x20

typedef struct
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int intn, err_code;                // Interrupt # and Error code (if applicable)
    unsigned int eip, cs, eflags, useresp, ss;  // on Interrupts CPU push this
} cpu_state_t;

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
struct idt_entry {
    uint16_t  offset_1;   // offset bits 0..15
    uint16_t  selector;   // a code segment selector in GDT or LDT
    uint8_t   ist;        // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t   type_attr;  // type and attributes
    uint16_t  offset_2;   // offset bits 16..31
} __attribute__ ((aligned (8), __packed__));

void setup_gdt();
void setup_idt();
void isr_install();

#define setup_x86() {setup_gdt(); setup_idt(); isr_install(); }

typedef void (*irq_callback_t)();
void irq_install(uint8_t irq, irq_callback_t callback);
typedef void (*isr_callback_t)();
void idt_entry_setup(uint8_t idx, uint32_t callback);

#define ISRN_no_error(isrn,callback) void \
                                  isr##isrn () { \
                                    __asm__ __volatile__("cli\n"); \
                                    __asm__ __volatile__("pushb $0\n"); \
                                    __asm__ __volatile__("pushb $##isrn\n"); \
                                    isr_handler(); \
}

#define IRQN(irqn,callback)     void irq##irqn () {         \
                                    callback();             \
                                    pic_acknowledge(irqn);  \
                                    __asm__ __volatile__("add $12, %esp\n");  \
                                    __asm__ __volatile__("iret\n");           \
                                  };                              \

#endif
