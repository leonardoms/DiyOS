#include <drivers/rtl81xx.h>

struct _rtl81xx_id rtl81xx_id[] = {
  { 0x10EC, 0x8168 }, // RTL-8029
  { 0, 0}
};

void rtl81xx_lock();
void rtl81xx_unlock();

struct x_descriptor   tx_descriptor[TX_DESCRIPTORS] __attribute__ ((aligned (256)));
struct x_descriptor   rx_descriptor[RX_DESCRIPTORS] __attribute__ ((aligned (256)));
uint8_t               tx_buffer[TX_BUFFER_SIZE];
uint8_t               rx_buffer[RX_BUFFER_SIZE];

uint16_t  rtl81xx_io_base;
uint8_t   rtl81xx_MAC[6];
uint8_t   rtl81xx_bus, rtl81xx_dev, rtl81xx_function;
uint8_t   rtl81xx_irq, rtl81xx_link_up;

void
rtl81xx_handler() {

  uint16_t status;

  status = inw( rtl81xx_io_base + 0x3E);

  if( status && RTL81XX_RX_OK) {
      printf("rx");
  } else
  if( status && RTL81XX_RX_ERR) {
      // Error on Recv Packet
  } else
  if( status && RTL81XX_TX_OK) {
      printf("tx");
  } else
  if( status && RTL81XX_RX_UNAVLB) {
      printf("RTL81xx: RX descriptors unavailable (%d packet loss)\n", inw(rtl81xx_io_base + 0x4C) && 0xFFF); // 24-bits packet loss count
  } else
  if( status && RTL81XX_LINK_CHG) {
      rtl81xx_link_up = inb(rtl81xx_io_base + 0x54) && 0x10;
      printf("RTL81xx: link ");
      if(rtl81xx_link_up)
        printf("up!\n");
      else
        printf("down!\n");
  } else
  if( status && RTL81XX_RX_OVERFLOW) {
      printf("RTL81xx: RX overflow (%d packet loss)\n", inw(rtl81xx_io_base + 0x4C) && 0xFFF); // 24-bits packet loss count
  } else
  if( status && RTL81XX_TX_UNAVLB) {
      printf("RTL81xx: TX descriptors unavailable\n"); // 24-bits packet loss count
  } else
  if( status && RTL81XX_SOFT_INT) {
      // software interrupt
  } else
  if( status && RTL81XX_TIMEOUT) {
      printf("RTL81xx: timeout.\n");
      rtl81xx_stop();
  } else
  if( status && RTL81XX_SYS_ERR) {
      printf("RTL81xx: has a hardware error. (stoping Rx/Tx)\n");
      rtl81xx_stop();
  } else {
      printf("RTL81xx: unknown reason Interrupt.\n");
  }

  // TODO: work with packages in buffer!
  pic_acknowledge(rtl81xx_irq);
  __asm__ __volatile__("add $12, %esp\n");
  __asm__ __volatile__("iret\n");
}

void
setup_rtl81xx() {
  uint32_t  i;
  uint8_t   rtl81xx_present;

  printf("Realtek 81xx network card: ");

  rtl81xx_present = 0;
  i = 0;
  while(rtl81xx_id[i].vendor) {
    if(pci_find(rtl81xx_id[i].vendor, rtl81xx_id[i].device, &rtl81xx_bus,
                &rtl81xx_dev, &rtl81xx_function)) {
        rtl81xx_present = 1;
        break;
      }
    i++;
  }

  if(rtl81xx_present == 0) {
    printf("not found\n");
    return;
  }

  // okay, device was found

  //get IO base addr
  rtl81xx_io_base = pci_read(rtl81xx_bus, rtl81xx_dev, rtl81xx_function, PCI_BAR0);
  rtl81xx_io_base = rtl81xx_io_base & 0xFFFC; // 4-byte aligned

  rtl81xx_irq = pci_read(rtl81xx_bus, rtl81xx_dev, rtl81xx_function, 0x3C) & 0xFF;
  rtl81xx_reset();

  for(i = 0; i < 6; i++)
    rtl81xx_MAC[i] = inb(rtl81xx_io_base + i);

  // starts configuration

  outw(rtl81xx_io_base + 0xE0, 0x20); // configure C+ Command. no VLAN de-taging; Checksum offload.
  outb(rtl81xx_io_base + 0x37, 0x00);  // configure Command. disable RX/TX

  // setup rx descriptors
  for( i = 0; i < RX_DESCRIPTORS; i++) {
      rx_descriptor[i].cmd_status = 0x80000000 | (RX_BUFFER_SIZE & 0x3FFF);
      rx_descriptor[i].hi_mem = 0;
      rx_descriptor[i].low_mem = (uint32_t)VIRTUAL_TO_PHYSICAL(&rx_buffer);
  }
  rx_descriptor[i-1].cmd_status |= 0x40000000; // set End Of Ring flag on last entry

  // setup tx descriptors
  for( i = 0; i < TX_DESCRIPTORS; i++) {
      tx_descriptor[i].cmd_status = 0x80000000 | (TX_BUFFER_SIZE & 0x3FFF);
      tx_descriptor[i].hi_mem = 0;
      tx_descriptor[i].low_mem = (uint32_t)VIRTUAL_TO_PHYSICAL(&tx_buffer);
  }
  tx_descriptor[i-1].cmd_status |= 0x40000000; // set End Of Ring flag on last entry

  rtl81xx_unlock();   // allow modify config registers

  outb(rtl81xx_io_base + 0x3C, 0xFF); // enable all interrupts
  outb(rtl81xx_io_base + 0x3D, 0xFF);

  // about RX packets
  outw(rtl81xx_io_base + 0xDA, 0xC);   // max  tx packet size
  outl(rtl81xx_io_base + 0x44, 0xE70F); // accept anything (test)

  // about TX packets
  outw(rtl81xx_io_base + 0xEC, 0xC);   // max  tx packet size  in 32-bytes unit
  outl(rtl81xx_io_base + 0x41, 0x7);   // DMA size Unlimited

  // setup LED mode
  outb(rtl81xx_io_base + 0x52, 0xC0); // configure LED mode 4 (rx/tx activity)

  outw(rtl81xx_io_base + 0xE4, (uint32_t)VIRTUAL_TO_PHYSICAL(&rx_descriptor));

  outb(rtl81xx_io_base + 0x37, 0x0C);  // configure Command. enable RX/TX
  rtl81xx_lock();

  irq_install(rtl81xx_irq, rtl81xx_handler);


  printf("\n\tio_base=0x%x, irq=%d", rtl81xx_io_base, rtl81xx_irq);
  printf("\n\tMAC=%x:%x:%x:%x:%x:%x\n", rtl81xx_MAC[0],rtl81xx_MAC[1],
                                        rtl81xx_MAC[2],rtl81xx_MAC[3],
                                        rtl81xx_MAC[4],rtl81xx_MAC[5]);
}

void
rtl81xx_reset() {
    outb(rtl81xx_io_base + 0x37, 0x10);
    while(inb(rtl81xx_io_base + 0x37) && 0x10);
}

void
rtl81xx_stop() {
    rtl81xx_unlock();
    outb(rtl81xx_io_base + 0x37, 0x00);
    rtl81xx_lock();
}

void
rtl81xx_start() {
    rtl81xx_unlock();
    outb(rtl81xx_io_base + 0x37, 0x0C);
    rtl81xx_lock();
}

void
rtl81xx_unlock() {
    outb(rtl81xx_io_base + 0x50, 0xC0);
}

void
rtl81xx_lock() {
    outb(rtl81xx_io_base + 0x50, 0x0);
}

uint8_t
rtl81xx_link_is_up() {
    return inb(rtl81xx_io_base + 0x54) && 0x10;
}

void
rtl81xx_read_packet(uint8_t* payload, uint32_t* payload_size) {

}

void
rtl81xx_write_packet(uint8_t* payload, uint32_t payload_size) {

}
