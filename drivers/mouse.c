#include <stdio.h>
#include <drivers/mouse.h>

queue_t mouse_queue;
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

void
mouse_listen(task_t* t, void* data) {
  if(t->listen && MOUSE) {
    message_to(t->id, data, sizeof(struct mouse_packet)); // send mouse packet
  }
}

void
mouse_task(void) {
  static struct mouse_packet* packet;

  while(1) {

      scheduling(0);

      while( ( packet = (struct mouse_packet*)queue_remove(&mouse_queue) ) != NULL ) {

          task_foreach( mouse_listen, (packet) );
      }
      scheduling(1);

      task_block(); // block 'mouse' task
  }
}

void
mouse_handler(void) {

  static uint8_t status, cicle = 0;
  static struct mouse_packet pkt;

  asm volatile("cli");
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

        if( (pkt.flags & 0x80) || (pkt.flags & 0x40) == 0 ) { // x/y overflow ?
          packet = (struct mouse_packet*)malloc(sizeof(struct mouse_packet));

          // if(pkt.flags & 0x20) {  // is dY negative?
          //   pkt.dy =
          // }

          memcpy(&pkt, packet, sizeof(struct mouse_packet));

          queue_add(&mouse_queue, packet);
          task_wake(m_task);
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

  asm volatile("popa");
  asm volatile("iret");
}

void
mouse(void) {
  uint8_t status;

  queue_init(&mouse_queue, 32);

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

  m_task = task_create(mouse_task, "mouse", TS_BLOCKED);
  m_task->id = MOUSE; // force mouse pid
  task_add(m_task);

  irq_enable(12);

}
