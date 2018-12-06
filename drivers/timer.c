
#include <drivers/timer.h>
#include <kernel.h>

#define HZ_FREQUENCY  100

uint32_t current_tick;
static uint8_t _scheduling = 0;

void
update_timeout(task_t* t, uint32_t* data) {
  if(t->timeout > 0) {
    t->timeout -= (int32_t)data;
    if( t->timeout <= 0 ) {
      t->state = TS_READY;
      t->timeout = 0;
    }
      // printf("%s (%d) %dms left for timeout.\n", t->name, t->state, t->timeout);
  }

}

void
scheduling(uint8_t state) {
  _scheduling = (state != 0);
}

void
timer_handler(void) {
  asm volatile("cli");
  asm volatile("add $0xc, %esp");

  if(_scheduling) {
    task_foreach(update_timeout, (uint32_t*)(1000/HZ_FREQUENCY));
  	task_schedule();
  } else {

    asm volatile("pusha");
    pic_acknowledge(0);
    asm volatile("popa");
    asm volatile("iret");
  }
}

void
timer(){
  unsigned short f = 1193181 / HZ_FREQUENCY; // 100 = 100hz -> 0.01s
  outportb(0x43, 0x36);
  outportb(0x40, f & 0xFF);
  outportb(0x40, f >> 8);

  irq_enable(0);
  scheduling(0);
}
