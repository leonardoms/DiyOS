#include <stdio.h>
#include <queue.h>

queue_t mouse_queue;

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

struct mouse_packet {
    uint8_t flags;
    uint8_t dx;
    uint8_t dy;
};

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
        // message_to()
        printf("mouse packet: flags=0x%x, dx=%d, dy=%d\n", pkt.flags, pkt.dx, pkt.dy);
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

  irq_enable(12);

}
