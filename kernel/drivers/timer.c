
#include <drivers/timer.h>
#include <small.h>

#define HZ_FREQUENCY  1000

void timer_handle() {
  static uint8_t state = 0;
  static uint32_t count = 0;

  if(count == HZ_FREQUENCY) {
    if(!state)
       printf("tic...");
    else
       printf("tac!\n");
    count = 0;
    state = ~state;
  } else
  count++;
}
IRQN(0,timer_handle); // create irq0 function

void setup_timer(){
  unsigned short f = 1193181 / HZ_FREQUENCY; // 100 = 100hz -> 0.01s
  outportb(0x43, 0x36);
  outportb(0x40, f & 0xFF);
  outportb(0x40, f >> 8);
  irq_install(0,irq0);
}
