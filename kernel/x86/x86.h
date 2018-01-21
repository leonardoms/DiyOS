#ifndef _X86_H_
#define _X86_H_

#include "ctypes.h"
#include "pic.h"

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
} __attribute__ ((aligned (8)));

void setup_gdt();
void setup_idt();

#define setup_x86() {setup_gdt(); setup_idt();}

typedef void (*irq_callback_t)();
void irq_install(uint8_t irq, irq_callback_t callback);
typedef void (*isr_callback_t)();

#define ISRN(isrn,callback,error) void isr##isrn () {       \
                                    __asm__ __volatile__("cli\n");
#if error == 0
                                    __asm__ __volatile__("pushb $0\n");
#endif
                                    __asm__ __volatile__("pushb $##isrn\n");
                                    isr_handler()

}

#define IRQN(irqn,callback)     void irq##irqn () {         \
                                    callback();             \
                                    pic_acknowledge(irqn);  \
                                    __asm__ __volatile__("add $12, %esp\n");  \
                                    __asm__ __volatile__("iret\n");           \
                                  } __attribute__ ((section (".irq")));                              \

#endif
