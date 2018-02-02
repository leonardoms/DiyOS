
#include <x86/x86.h>

void
isr0() {
    printf("kernel: Divide by 0\n");
    while(1);
}

void
isr2() {
    printf("kernel: NMI Interrupt\n");
    while(1);
}

void
isr3() {
    printf("kernel: Breakpoint (INT3)\n");
    while(1);
}

void
isr4() {
    printf("kernel: Overflow (INTO)\n");
    while(1);
}

void
isr5() {
    printf("kernel: Bounds range exceeded (BOUND)\n");
    while(1);
}

void
isr6() {
    printf("kernel: Invalid opcode (UD2)\n");
    while(1);
}

void
isr7() {
    printf("kernel: Device not available (WAIT/FWAIT)\n");
    while(1);
}

void
isr8() {
    printf("kernel: Double fault\n");
    while(1);
}

void
isr9() {
    printf("kernel: Coprocessor segment overrun\n");
    while(1);
}

void
isr10() {
    printf("kernel: Invalid TSS\n");
    while(1);
}

void
isr11() {
    printf("kernel: Segment not present\n");
    while(1);
}

void
isr12() {
    printf("kernel: Stack-segment fault\n");
    while(1);
}

void
isr13() {
    printf("kernel: General protection fault\n");
    while(1);
}

void
isr14() {
    printf("kernel: Page fault\n");
    while(1);
}


void
isr16() {
    printf("kernel: x87 FPU error\n");
    while(1);
}

void
isr17() {
    printf("kernel: Alignment check\n");
    while(1);
}

void
isr18() {
    printf("kernel: Machine check\n");
    while(1);
}

void
isr19() {
    printf("kernel: SIMD Floating-Point Exception\n");
    while(1);
}

void
isr_install() {
  idt_entry_setup(0, (uint32_t)isr0);
  idt_entry_setup(2, (uint32_t)isr2);
  idt_entry_setup(3, (uint32_t)isr3);
  idt_entry_setup(4, (uint32_t)isr4);
  idt_entry_setup(5, (uint32_t)isr5);
  idt_entry_setup(6, (uint32_t)isr6);
  idt_entry_setup(7, (uint32_t)isr7);
  idt_entry_setup(8, (uint32_t)isr8);
#if 1
  idt_entry_setup(9, (uint32_t)isr9);
  idt_entry_setup(10, (uint32_t)isr10);
  idt_entry_setup(11, (uint32_t)isr11);
  idt_entry_setup(12, (uint32_t)isr12);
  idt_entry_setup(13, (uint32_t)isr13);
  idt_entry_setup(14, (uint32_t)isr14);
  idt_entry_setup(16, (uint32_t)isr16);
  idt_entry_setup(17, (uint32_t)isr17);
  idt_entry_setup(18, (uint32_t)isr18);
  idt_entry_setup(19, (uint32_t)isr19);
#endif
}
