
#include <arch.h>
#include <kernel.h>


extern void load_idt();

isr_call_vector_t   isr_call_vector;
isr_call_vector_t   *isr_call_ptr_vector = 0;

struct fault_name {
    char* name;
    uint8_t has_errcode;
} fault_names[] = {
  {"Divide-by-Zero", FALSE },
  {"Debug", FALSE},
  {"Non-maskable Interrupt", FALSE},
  {"Breakpoint", FALSE},
  {"Overflow", FALSE},
  {"Bound Range Exceeded", FALSE},
  {"Invalid Opcode", FALSE},
  {"Device Not Available", FALSE},
  {"Double Fault", TRUE},
  {"Coprocessor Segment Overrun", FALSE},
  {"Invalid TSS", TRUE},
  {"Segment Not Present", TRUE},
  {"Stack-Segment Fault", TRUE},
  {"General Protection Fault", TRUE},
  {"Page Fault", TRUE},
  {NULL, FALSE},
  {"x87 Floating-Point Exception", FALSE},
  {"Alignment Check", TRUE},
  {"Machine Check", FALSE},
  {"SIMD Floating-Point Exception", FALSE},
  {"Virtualization Exception", FALSE},
  {NULL, FALSE},
  {NULL, FALSE},
  {NULL, FALSE},
  {NULL, FALSE},
  {NULL, FALSE},
  {NULL, FALSE},
  {NULL, FALSE},
  {NULL, FALSE},
  {NULL, FALSE},
  {"Security Exception", TRUE},
  {NULL, FALSE}
};

uint32_t
isr_fault(int_regs_t* regs) {
    uint32_t cr2;

    task_t* t;
    t = task_get();
    printf("********************************************************************\n");
    if(t) {
      printf("*\tPID: %d\tNAME: %s\n", t->pid, t->name );
    } else {
      printf("*\tPID: -\tNAME: -\n" );
    }

    __asm__ __volatile__("mov %%cr2, %%eax; mov %%eax, %0":"=m"(cr2)::"%eax");
    printf("*\tError! %s (error: 0x%x)\n", fault_names[regs->intn].name, regs->err_code);
    printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx );
    printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs->cs, regs->eip, regs->ss, regs->useresp );
    printf("********************************************************************\n");

#if 1

    t = task_get();
    debug_printf("********************************************************************\n");
    if(t) {
      debug_printf("*\tPID: %d\tNAME: %s\n", t->pid, t->name );
    } else {
      debug_printf("*\tPID: -\tNAME: -\n" );
    }

    __asm__ __volatile__("mov %%cr2, %%eax; mov %%eax, %0":"=m"(cr2)::"%eax");
    debug_printf("*\tError! %s (error: 0x%x)\n", fault_names[regs->intn].name, regs->err_code);
    debug_printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx );
    debug_printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs->cs, regs->eip, regs->ss, regs->useresp );
    debug_printf("********************************************************************\n");

#endif

    if(t)
      task_exit(1);
    //
    // while(1)
    //   __asm__ __volatile__("cli\n");
}

void
page_fault_handler(int_regs_t* regs) {
    uint32_t cr2, cr3;
    task_t* t = task_get();
    __asm__ __volatile__("mov %%cr2, %%eax; mov %%eax, %0":"=m"(cr2)::"%eax");
    __asm__ __volatile__("mov %%cr3, %%eax; mov %%eax, %0":"=m"(cr3)::"%eax");

    if(t) {
      printf("********************************************************************\n");
      printf("*\tPID: %d\tNAME: %s\n", t->pid, t->name );
      printf("*\tPAGEFAULT on address 0x%x (%s %s %s) !\n", cr2, (regs->err_code&1)?"Prot.":" ",
                                                    (regs->err_code&2)?"Write":"Read",
                                                    (regs->err_code&5)?"User":"Super" );
      printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx );
      printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs->cs, regs->eip, regs->ss, regs->useresp );
      printf("********************************************************************\n");
      // task_destroy();
    } else {
      printf("********************************************************************\n");
      printf("*\tPID: -\tNAME: (null)\n" );
      printf("*\tPAGEFAULT on address 0x%x (%s %s %s) !\n", cr2, (regs->err_code&1)?"Prot.":" ",
                                                    (regs->err_code&2)?"Write":"Read",
                                                    (regs->err_code&5)?"User":"Super" );
      printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx );
      printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs->cs, regs->eip, regs->ss, regs->useresp );
      printf("********************************************************************\n");
      PANIC("kernel fault.");
    }

  #if 1

  if(t) {
    debug_printf("********************************************************************\n");
    debug_printf("*\tPID: %d\tNAME: %s\n", t->pid, t->name );
    debug_printf("*\tPAGEFAULT on address 0x%x (%s %s %s) !\n", cr2, (regs->err_code&1)?"Prot.":" ",
                                                  (regs->err_code&2)?"Write":"Read",
                                                  (regs->err_code&5)?"User":"Super" );
    debug_printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx );
    debug_printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs->cs, regs->eip, regs->ss, regs->useresp );
    debug_printf("********************************************************************\n");
    // task_destroy();
  } else {
    debug_printf("********************************************************************\n");
    debug_printf("*\tPID: -\tNAME: (null)\n" );
    debug_printf("*\tPAGEFAULT on address 0x%x (%s %s %s) !\n", cr2, (regs->err_code&1)?"Prot.":" ",
                                                  (regs->err_code&2)?"Write":"Read",
                                                  (regs->err_code&5)?"User":"Super" );
    debug_printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx );
    debug_printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs->cs, regs->eip, regs->ss, regs->useresp );
    debug_printf("********************************************************************\n");
    PANIC("kernel fault.");
  }

  #endif

    while(1)
      __asm__ __volatile__("cli\n");
}

void irq7_handler(void) {

  // do nothing
  debug_printf("irq7\n");

}

void
isr_install() {

  ISR_SET_HANDLER(14, (uint32_t)page_fault_handler);
  IRQ_SET_HANDLER(7,irq7_handler);

}

void idt() {
  pic();

  load_idt();

  isr_call_ptr_vector = &isr_call_vector;

  isr_install();
}
