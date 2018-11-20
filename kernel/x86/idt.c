
#include <x86/x86.h>
#include <task.h>

extern void load_idt(struct idt* addr);
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();
extern void isr66();
extern void isr255();
extern void timer_handle();
extern void keyboard_handle();
extern void task_schedule_handler();

isr_callback_t isr_callbacks[256] __attribute__ ((aligned (8)));
idt_entry_t    idt[256];
struct idt     idtr;

void
isr_dispacher(isr_regs_t regs) {
    isr_callback_t c = isr_callbacks[regs.intn];

    if(c) {
      c(regs);
    } else {
      printf("isr_dispacher: undefined callback for #%x !\n", regs.intn);
    }
    // printf("ISR #%d (errorcode: %d)\n", regs.intn, regs.err_code);
    // while(1);
}

void
irq_dispacher(isr_regs_t regs) {
    isr_callback_t c = isr_callbacks[regs.intn];

    pic_acknowledge(regs.intn - IRQ_BASE);

    if(c) {
      c(regs);
    } else {
      printf("isr_dispacher: undefined callback for IRQ#%x !\n", regs.intn - IRQ_BASE);
    }
}

void
isr_install_callback(uint8_t intn, isr_callback_t callback) {

    if(callback == NULL) {
        printf("isr_install_callback: null callback\n");
        return;
    }

    isr_callbacks[intn] = callback;
}

void
irq_install_callback(uint8_t irq, isr_callback_t callback) {

  if(irq > 15) {
      printf("irq_install_callback: invalid irq number (%d)\n", irq);
  }

  if(callback == NULL) {
      printf("irq_install_callback: null callback (%d)\n", irq);
      return;
  }

  isr_install_callback(IRQ_BASE + irq, callback);
  irq_enable(irq);
}

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

void
fault_handler(isr_regs_t regs) {
    uint32_t cr2;

    task_t* t;
    t = task_get();
    printf("********************************************************************\n");
    if(t) {
      printf("*\tPID: %d\tNAME: %s\n", t->id, t->name );
    } else {
      printf("*\tPID: -\tNAME: -\n" );
    }

    __asm__ __volatile__("mov %%cr2, %%eax; mov %%eax, %0":"=m"(cr2)::"%eax");
    printf("*\tError! %s (error: 0x%x)\n", fault_names[regs.intn].name, regs.err_code);
    printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs.eax, regs.ebx, regs.ecx, regs.edx );
    printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs.cs, regs.eip, regs.ss, regs.useresp );
    printf("********************************************************************\n");

    if(t)
      task_destroy();

    while(1)
      __asm__ __volatile__("cli\n");
}

void
page_fault_handler(isr_regs_t regs) {
    uint32_t cr2, cr3;
    task_t* t = task_get();
    __asm__ __volatile__("mov %%cr2, %%eax; mov %%eax, %0":"=m"(cr2)::"%eax");
    __asm__ __volatile__("mov %%cr3, %%eax; mov %%eax, %0":"=m"(cr3)::"%eax");

    if(t) {
      printf("********************************************************************\n");
      printf("*\tPID: %d\tNAME: %s\n", t->id, t->name );
      printf("*\tPAGEFAULT on address 0x%x (%s %s %s) !\n", cr2, (regs.err_code&1)?"Prot.":" ",
                                                    (regs.err_code&2)?"Write":"Read",
                                                    (regs.err_code&5)?"User":"Super" );
      printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs.eax, regs.ebx, regs.ecx, regs.edx );
      printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs.cs, regs.eip, regs.ss, regs.useresp );
      printf("********************************************************************\n");
      task_destroy();
    } else {
      printf("********************************************************************\n");
      printf("*\tPID: -\tNAME: (null)\n" );
      printf("*\tPAGEFAULT on address 0x%x (%s %s %s) !\n", cr2, (regs.err_code&1)?"Prot.":" ",
                                                    (regs.err_code&2)?"Write":"Read",
                                                    (regs.err_code&5)?"User":"Super" );
      printf("*\tEAX=0x%x\tEBX=0x%x\tECX=0x%x\tEDX=0x%x\n", regs.eax, regs.ebx, regs.ecx, regs.edx );
      printf("*\tCS:EIP=0x%x:0x%x\tSS:ESP=0x%x:0x%x\n", regs.cs, regs.eip, regs.ss, regs.useresp );
      printf("********************************************************************\n");
    }

    while(1)
      __asm__ __volatile__("cli\n");
}


void
isr_install() {
  isr_install_callback(0, fault_handler);
  isr_install_callback(2, fault_handler);
  isr_install_callback(3, fault_handler);
  isr_install_callback(4, fault_handler);
  isr_install_callback(5, fault_handler);
  isr_install_callback(6, fault_handler);
  isr_install_callback(7, fault_handler);
  isr_install_callback(8, fault_handler);
  isr_install_callback(9, fault_handler);
  isr_install_callback(10, fault_handler);
  isr_install_callback(11, fault_handler);
  isr_install_callback(12, fault_handler);
  isr_install_callback(13, fault_handler);
  isr_install_callback(14, page_fault_handler);
  isr_install_callback(16, fault_handler);
  isr_install_callback(17, fault_handler);
  isr_install_callback(18, fault_handler);
  isr_install_callback(19, fault_handler);
}

void irq7_handler(isr_regs_t regs) {
  // do nothing
}

void setup_idt_entry(uint8_t intn, uint32_t isr_addr, uint8_t dpl) {
      idt_entry_t* ie = &idt[intn];
      ie->offset_1 = isr_addr & 0xFFFF;
      ie->offset_2 = (isr_addr >> 16) & 0xFFFF;
      ie->selector = 0x08;  // kernel code
      ie->ist = 0x00;
      ie->type_attr = 0x8E | (dpl << 5); // present, 32-bits interrupt, DPL = 0
}

void setup_idt() {
  setup_pic();
  // make sure that all idt entries are zeroed
  uint32_t* e;
  e = (uint32_t*)idt;
  uint32_t i;
  for(i = 0; i < 512; i++)
      e[i] = 0x00000000;

  setup_idt_entry(0, (uint32_t)isr0, 0);
  setup_idt_entry(1, (uint32_t)isr1, 0);
  setup_idt_entry(3, (uint32_t)isr3, 0);
  setup_idt_entry(4, (uint32_t)isr4, 0);
  setup_idt_entry(5, (uint32_t)isr5, 0);
  setup_idt_entry(6, (uint32_t)isr6, 0);
  setup_idt_entry(7, (uint32_t)isr7, 0);
  setup_idt_entry(8, (uint32_t)isr8, 0);
  setup_idt_entry(9, (uint32_t)isr9, 0);
  setup_idt_entry(10, (uint32_t)isr10, 0);
  setup_idt_entry(11, (uint32_t)isr11, 0);
  setup_idt_entry(12, (uint32_t)isr12, 0);
  setup_idt_entry(13, (uint32_t)isr13, 0);
  setup_idt_entry(14, (uint32_t)isr14, 0);
  setup_idt_entry(15, (uint32_t)isr15, 0);
  setup_idt_entry(16, (uint32_t)isr16, 0);
  setup_idt_entry(17, (uint32_t)isr17, 0);
  setup_idt_entry(18, (uint32_t)isr18, 0);
  setup_idt_entry(19, (uint32_t)isr19, 0);
  setup_idt_entry(20, (uint32_t)isr20, 0);
  setup_idt_entry(21, (uint32_t)isr21, 0);
  setup_idt_entry(22, (uint32_t)isr22, 0);
  setup_idt_entry(23, (uint32_t)isr23, 0);
  setup_idt_entry(24, (uint32_t)isr24, 0);
  setup_idt_entry(25, (uint32_t)isr25, 0);
  setup_idt_entry(26, (uint32_t)isr26, 0);
  setup_idt_entry(27, (uint32_t)isr27, 0);
  setup_idt_entry(28, (uint32_t)isr28, 0);
  setup_idt_entry(29, (uint32_t)isr29, 0);
  setup_idt_entry(30, (uint32_t)isr30, 0);
  setup_idt_entry(31, (uint32_t)isr31, 0);

  // IRQ
  setup_idt_entry(32, (uint32_t)timer_handle, 0);
  setup_idt_entry(33, (uint32_t)keyboard_handle, 0);
  printf("keyboard_handle: 0x%x\n", (uint32_t)keyboard_handle);
  setup_idt_entry(34, (uint32_t)isr34, 0);
  setup_idt_entry(35, (uint32_t)isr35, 0);
  setup_idt_entry(36, (uint32_t)isr36, 0);
  setup_idt_entry(37, (uint32_t)isr37, 0);
  setup_idt_entry(38, (uint32_t)isr38, 0);
  setup_idt_entry(39, (uint32_t)isr39, 0);
  setup_idt_entry(40, (uint32_t)isr40, 0);
  setup_idt_entry(41, (uint32_t)isr41, 0);
  setup_idt_entry(42, (uint32_t)isr42, 0);
  setup_idt_entry(43, (uint32_t)isr43, 0);
  setup_idt_entry(44, (uint32_t)isr44, 0);
  setup_idt_entry(45, (uint32_t)isr45, 0);
  setup_idt_entry(46, (uint32_t)isr46, 0);
  setup_idt_entry(47, (uint32_t)isr47, 0);

  // syscall
  setup_idt_entry(66, (uint32_t)isr66, 3);      // dpl = 3

  setup_idt_entry(0xFF, (uint32_t)task_schedule_handler, 0);

  irq_install_callback(7,irq7_handler);
  isr_install();

  idtr.size = 8 * 256;
  idtr.offset = (uint32_t)&idt[0];

  load_idt(&idtr);
}
