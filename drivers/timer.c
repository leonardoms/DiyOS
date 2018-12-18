
#include <drivers/timer.h>
#include <kernel.h>

#define HZ_FREQUENCY  100

uint8_t
update_timeout(task_t* t, uint32_t* data) {
  if(t->timeout > 0) {
    t->timeout -= (int32_t)data;
    if( t->timeout <= 0 ) {
      t->status = TS_READY;
      t->timeout = 0;
    }
      // printf("%s (%d) %dms left for timeout.\n", t->name, t->state, t->timeout);
  }
  return 0;
}

uint32_t
timer_handler(int_regs_t* regs) {
  pic_acknowledge(0);

  task_foreach(update_timeout, (uint32_t*)(1000/HZ_FREQUENCY));

	schedule();

  debug_printf("return from timer_handler\n");
}

void
timer(){
  unsigned short f = 1193181 / HZ_FREQUENCY; // 100 = 100hz -> 0.01s
  outportb(0x43, 0x36);
  outportb(0x40, f & 0xFF);
  outportb(0x40, f >> 8);

  IRQ_SET_HANDLER(0, timer_handler);
  irq_enable(0);
}
