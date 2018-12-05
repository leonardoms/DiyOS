#include <stdio.h>
#include <drivers/mouse.h>

queue_t mouse_queue;
task_t* m_task;

void
mouse_wait(uint8_t type) {
    uint32_t time_out = 100000;

    if( type == 0 ) {
        while( time_out-- ) {
            if( (inportb(0x64) & 1) == 1 )
                return;
        }
        return;
    } else
    {
        while( time_out-- ) {
            if( (inportb(0x64) & 2) == 0 ) {
                return;
            }
        }
        return;
    }
}

void
mouse_write(uint8_t write)
{
    mouse_wait(1);
    outportb(0x64, 0xD4);

    mouse_wait(1);
    outportb(0x60, write);
}

uint8_t
mouse_read()
{
    mouse_wait(0);
    return inportb(0x60);
}

struct mouse_packet* packet;

void
mouse_listen(task_t* t, uint32_t* data) {
  if(t->listen && MOUSE) {
    message_to(t->id, data, 0); // send mouse packet
  }
}

void
mouse_task(void) {
  static struct mouse_packet* packet;

  while(1) {

      scheduling(0);

      while( ( packet = (struct mouse_packet*)queue_remove(&mouse_queue) ) != NULL ) {

          task_queue_foreach(&tq_ready, mouse_listen, (uint32_t*)(packet) );
          task_queue_foreach(&tq_blocked, mouse_listen, (uint32_t*)(packet));
      }
      scheduling(1);

      task_block(); // block 'mouse' task
  }
}

void
mouse_handler(void) {

  static uint8_t status, cicle = 0;
  static struct mouse_packet pkt;

  asm volatile("add $0xc, %esp");
  asm volatile("pusha");

  status = inportb(0x64);

  if ( (status & 0x01) && (status & 0x20) ) {
    switch (cicle) {
      case 0:
        pkt.flags = mouse_read();
        cicle++;
        break;
      case 1:
        pkt.dx = mouse_read();
        cicle++;
        break;
      case 2:
        pkt.dy = mouse_read();

        packet = (struct mouse_packet*)malloc(sizeof(struct mouse_packet));
        memcpy(&pkt, packet, sizeof(struct mouse_packet));

        queue_add(&mouse_queue, packet);
        task_wake(m_task);
        // printf("mouse packet: flags=0x%x, dx=%d, dy=%d\n", pkt.flags, pkt.dx, pkt.dy);
        cicle = 0;
        break;
      default:
        cicle = 0;
        break;
    }
  }

  pic_acknowledge(12);

  asm volatile("popa");
  asm volatile("iret");
}

void
mouse(void) {
  uint8_t status;

  queue_init(&mouse_queue, 32);

  mouse_wait(1);
  outportb(0x64, 0xA8);

  //Enable the interrupts
  mouse_wait(1);
  outportb(0x64, 0x20);

  mouse_wait(0);
  status = (inportb(0x60) | 2);

  mouse_wait(1);
  outportb(0x64, 0x60);

  mouse_wait(1);
  outportb(0x60, status);

  // Tell the mouse to use default settings
  mouse_write(0xF6);
  mouse_read();  // ACK

  // Enable the mouse
  mouse_write(0xF4);
  mouse_read();  // ACK

  m_task = task_create(mouse_task, "mouse", TS_BLOCKED);
  m_task->id = MOUSE; // force mouse pid
  task_add(m_task);

  irq_enable(12);

}
