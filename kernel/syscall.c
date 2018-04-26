
#include <x86/x86.h>

void
syscall(irq_regs_t regs) {

    switch (regs.eax) {
      case 1:
          putchar((char)(regs.ebx));
          break;
      default:
          printf("syscall: unknown id %d\n", regs.eax);
          break;
    }

}

void
syscall_setup() {
    isr_install_callback(0x42, syscall, 3);
}
