
#include <kernel.h>
#include <stdio.h>

uint16_t SERIAL_PORT[4];
const uint8_t  *serial_name[4] = { {"com1"}, {"com2"}, {"com3"}, {"com4"}  };

static uint8_t serial_lock = 1;

uint8_t
serial_received(uint8_t id) {
   return (uint8_t)(inportb(SERIAL_PORT[id] + 5) & 1);
}

uint8_t
read_serial(uint8_t id) {
   while (serial_received(id) == 0);

   return inportb(SERIAL_PORT[id]);
}

void
write_serial(uint8_t id, uint8_t c) {
   while ( (inportb(SERIAL_PORT[id] + 5) & 0x20) == 0);

   outportb(SERIAL_PORT[id],c);
}

void
serial_handler() {
  asm volatile("cli");
  asm volatile("add $0xc, %esp");
  asm volatile("pusha");

  serial_lock = 0; // used in read_fs

  pic_acknowledge(3);
  pic_acknowledge(4);

  asm volatile("popa");
  asm volatile("iret");
}


uint32_t
serial_write_fs(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {

  uint32_t i = 0;

  // printf("/dev/%s write (%d bytes).\n", serial_name[node->inode], size);

  while( i < size ) {
    write_serial(node->inode, buffer[i++]);
  }

  return i;
}

void
serial() {
  fs_node_t* com;
  uint32_t i;

  if( (SERIAL_PORT[0] = bios_read_word(0x40, 0x0)) == 0 )
    SERIAL_PORT[0] = 0x3F8;

  if( (SERIAL_PORT[1] = bios_read_word(0x40, 0x2)) == 0 )
    SERIAL_PORT[1] = 0x2F8;

  if( (SERIAL_PORT[2] = bios_read_word(0x40, 0x4)) == 0 )
    SERIAL_PORT[2] = 0x3E8;

  if( (SERIAL_PORT[3] = bios_read_word(0x40, 0x6)) == 0 )
    SERIAL_PORT[3] = 0x2E8;

  for( i = 0; i < 4; i++ ) {
    // if(SERIAL_PORT[i] == 0)
    //   continue;

    outportb(SERIAL_PORT[i] + 1, 0x00);    // Disable all interrupts
    outportb(SERIAL_PORT[i] + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outportb(SERIAL_PORT[i] + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outportb(SERIAL_PORT[i] + 1, 0x00);    //                  (hi byte)
    outportb(SERIAL_PORT[i] + 3, 0x03);    // 8 bits, no parity, one stop bit
    outportb(SERIAL_PORT[i] + 2, 0x7);    // Enable FIFO, clear them, with 1-byte threshold
    // outportb(SERIAL_PORT[i] + 4, 0x0F);    // IRQs enabled, RTS/DSR set

    com = (fs_node_t*)malloc(sizeof(fs_node_t));
    strcpy(com->name, serial_name[i]);
    com->flags = FS_FILE;
    com->read = 0;
    com->write = &serial_write_fs;
    com->open = 0;
    com->close = 0;
    com->readdir = 0;
    com->finddir = 0;
    com->inode = i;

    devfs_register(com);
  }

  printf("Serial ports added to /dev/com[1-4]\n");
}
