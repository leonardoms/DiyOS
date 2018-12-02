
#include <kernel.h>

void
syscall(isr_regs_t regs) {
  // task_schedule_new();
task_t* t;
    switch (regs.eax) {
      case 10: //sleep
          t = task_get();
          ASSERT_PANIC(t != NULL);
          printf("put %s (%d) to sleep %ds\n", t->name, t->id, regs.ebx);
          t->timeout = regs.ebx * 1000;
          t->state = TS_BLOCKED;
          task_schedule(regs);
          break;
      case 0:
          task_destroy();
          break;
      case 1:
          putchar((char)(regs.ebx));
          break;
      case 2:
          t = task_get();
          ASSERT_PANIC(t != NULL);
          t->waitkey = 1;
          t->state = TS_BLOCKED;
          // printf("'%s' getchar()\n", t->name );
          task_schedule(regs);
          break;
      case 100:
          // frame_get();
          break;
      case 101:
          // frame_free();
          break;
      case 102:
          // frame_free_count();
          break;
      default:
          printf("syscall: unknown id %d\n", regs.eax);
          break;
    }

}

void
syscall_setup() {
    isr_install_callback(0x42, syscall);
}
