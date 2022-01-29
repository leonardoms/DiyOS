#include <stdio.h>
#include <drivers/mouse.h>

queue_t mouse_queue, mouse_wait;
task_t* m_task;

int32_t mouse_id;

void
_mouse_wait(uint8_t type) {
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
        debug_printf("mouse timed out!\n");
        return;
    }
}

void
mouse_write(uint8_t write)
{
    _mouse_wait(1);
    outportb(0x64, 0xD4);
    _mouse_wait(1);
    outportb(0x60, write);
}

uint8_t
mouse_read()
{
    _mouse_wait(0);
    char c = inportb(0x60);
    return c;
}

void
mouse_start() {
  uint8_t status;

  _mouse_wait(1);
	outportb(0x64, 0xA8);
	_mouse_wait(1);
	outportb(0x64, 0x20);
	_mouse_wait(0);
	status = inportb(0x60) | 2;
	_mouse_wait(1);
	outportb(0x64, 0x60);
	_mouse_wait(1);
	outportb(0x60, status);
	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();
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

struct mouse_packet* pkt;

uint32_t
mouse_handler(int_regs_t* regs) {

  uint8_t status, cicle;
  // static struct mouse_packet pkt;

  status = inportb(0x64);
  cicle = 0;

  while(status & 0x01) {

    int8_t mouse_in = inportb(0x60);

    if ( status & 0x20 ) {
      switch (cicle) {
        case 0:
          pkt->flags = mouse_in;
          if (!(mouse_in & 0x08)) return;
          ++cicle;
          break;
        case 1:
          pkt->dx = mouse_in;// (int32_t)mouse_read() - (int32_t)((pkt->flags << 27) & 0x80000000);
          ++cicle;
          break;
        case 2:
          pkt->dy = mouse_in;

          if( (pkt->flags & 0x80) || (pkt->flags & 0x40) )  // x/y overflow ?
            goto read_next;


          packet = (struct mouse_packet*)malloc(sizeof(struct mouse_packet)+4);

          memcpy(pkt, packet, sizeof(struct mouse_packet));

          queue_add(&mouse_queue, packet);
          // debug_printf("irq -> mouse packet: flags=0x%x, dx=%d, dy=%d, dz=%d\n", packet->flags, packet->dx, packet->dy, packet->dz);

          cicle = 0;
          break;
        }
      }
      read_next:
      status = inportb(0x64);
    }

  wake_up(&mouse_wait);
  pic_acknowledge(12);
}

void
mouse(void) {
  uint8_t status;


  pkt = (struct mouse_packet*)malloc(sizeof(struct mouse_packet)+4);
  queue_init(&mouse_queue, 32);
  queue_init(&mouse_wait, 8);

  mouse_start();

  mouse_write(0xF2);
  mouse_read();  // ACK
  mouse_id = mouse_read();
  debug_printf("mouse_id=%x\n",mouse_id);

  m_task = thread_create((uint32_t)mouse_task, 1, 100, "mouse");
  m_task->pid = MOUSE; // force mouse pid
  task_add(m_task);

  IRQ_SET_HANDLER(12, mouse_handler);
  irq_enable(12);

}
