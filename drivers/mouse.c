#include <stdio.h>
#include <drivers/mouse.h>

queue_t mouse_queue, mouse_wait;
task_t* m_task;

void
mouse_write(uint8_t write)
{
    outportb(0x64, 0xD4);
    outportb(0x60, write);
}

uint8_t
mouse_read()
{
    return inportb(0x60);
}

struct mouse_packet* packet;

uint8_t
mouse_listen(task_t* t, void* data) {
  if(t->listen && MOUSE) {
    message_to(t->pid, data, sizeof(struct mouse_packet)); // send mouse packet
  }

  return 0;
}

static void
mouse_task(void) {
  static struct mouse_packet* packet;

  while(1) {

      while( ( packet = (struct mouse_packet*)queue_remove(&mouse_queue) ) != NULL ) {

          task_foreach( mouse_listen, (packet) );
      }

      sleep_on(&mouse_wait, NULL); // block 'mouse' thread
  }
}

uint32_t
mouse_handler(int_regs_t* regs) {

  static uint8_t status, cicle = 0;
  static struct mouse_packet pkt;

  status = inportb(0x64);

  if ( (status & 0x01) && (status & 0x20) ) {
    switch (cicle) {
      case 0:
        pkt.flags = mouse_read();
        cicle++;
        break;
      case 1:
        if(pkt.flags & 0x10)  // is dY negative?
          pkt.dx = (int32_t) mouse_read() | 0xFFFFFF00;
        else
          pkt.dx = (int32_t) mouse_read();

        cicle++;
        break;
      case 2:

        if(pkt.flags & 0x20)  // is dY negative?
          pkt.dy = (int32_t) mouse_read() | 0xFFFFFF00;
        else
          pkt.dy = (int32_t) mouse_read();

        if( (pkt.flags & 0x80) || (pkt.flags & 0x40) == 0 ) { // x/y overflow ?
          packet = (struct mouse_packet*)malloc(sizeof(struct mouse_packet));

          // if(pkt.flags & 0x20) {  // is dY negative?
          //   pkt.dy =
          // }

          memcpy(&pkt, packet, sizeof(struct mouse_packet));

          queue_add(&mouse_queue, packet);
          wake_up(&mouse_wait);
        } // else { do nothing, discard entire packet :/ }
        // printf("mouse packet: flags=0x%x, dx=%d, dy=%d\n", pkt.flags, pkt.dx, pkt.dy);
        cicle = 0;
        break;
      default:
        cicle = 0;
        break;
    }
  }

  pic_acknowledge(12);

}

void
mouse(void) {
  uint8_t status;

  queue_init(&mouse_queue, 32);
  queue_init(&mouse_wait, 8);

  outportb(0x64, 0xA8);

  //Enable the interrupts
  outportb(0x64, 0x20);

  status = (inportb(0x60) | 2);

  outportb(0x64, 0x60);

  outportb(0x60, status);

  // Tell the mouse to use default settings
  mouse_write(0xF6);
  mouse_read();  // ACK

  // Enable the mouse
  mouse_write(0xF4);
  mouse_read();  // ACK

  m_task = thread_create((uint32_t)mouse_task, 1, 100, "mouse");
  m_task->pid = MOUSE; // force mouse pid
  task_add(m_task);

  IRQ_SET_HANDLER(12, mouse_handler);
  irq_enable(12);

}
