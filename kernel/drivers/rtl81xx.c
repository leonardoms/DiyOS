#include <drivers/rtl81xx.h>


struct _rtl81xx_id {
  uint16_t  vendor;
  uint16_t  device;
} rtl81xx_id[] = {
  { 0x10EC, 0x8168 }, // RTL-8029
  { 0, 0}
};

void rtl81xx_lock();
void rtl81xx_unlock();

struct x_descriptor   tx_descriptor[TX_DESCRIPTORS];
struct x_descriptor   rx_descriptor[RX_DESCRIPTORS];
uint8_t               tx_buffer[TX_DESCRIPTORS][TX_BUFFER_SIZE] __attribute__ ((aligned (8)));
uint8_t               rx_buffer[RX_DESCRIPTORS][RX_BUFFER_SIZE] __attribute__ ((aligned (8)));;

uint16_t  rtl81xx_io_base;
uint8_t   rtl81xx_MAC[6];
uint8_t   rtl81xx_bus, rtl81xx_dev, rtl81xx_function;
uint8_t   rtl81xx_irq, rtl81xx_link_up;

static void
rtl81xx_handler() {

  uint16_t status;
  uint32_t i;

  status = inw(rtl81xx_io_base + 0x3E);

  // printf("rtl81xx_handler(): status 0x%x\n", status);

  if( status != 0xFFFF )  // ignore weird status
  if( status & RTL81XX_RX_OK) {
      // packet received
      for( i = 0; i < RX_DESCRIPTORS; i++ ) { // check for descriptors with packets
        if(rx_descriptor[i].cmd_status & 0x80000000) // check Owner bit
          continue; // skip descriptor OWNed by NIC
        printf("Rx size(%d)\n", rx_descriptor[i].cmd_status & 0x3FFF);

        if(rx_descriptor[i].cmd_status & 0x60000) { // is TCP/IP ?
            printf("Rx from %d.%d.%d.%d to %d.%d.%d.%d - %d bytes\n", // eg.: Rx from 192.168.0.255 to 192.168.0.5 - 96 bytes
                        ((uint8_t*)PHYSICAL_TO_VIRTUAL(rx_descriptor[i].low_mem))[0], // read
                        ((uint8_t*)PHYSICAL_TO_VIRTUAL(rx_descriptor[i].low_mem))[1], // data
                        ((uint8_t*)PHYSICAL_TO_VIRTUAL(rx_descriptor[i].low_mem))[2], // from
                        ((uint8_t*)PHYSICAL_TO_VIRTUAL(rx_descriptor[i].low_mem))[3], // TCP/IP
                        ((uint8_t*)PHYSICAL_TO_VIRTUAL(rx_descriptor[i].low_mem))[4], // header
                        ((uint8_t*)PHYSICAL_TO_VIRTUAL(rx_descriptor[i].low_mem))[5], // on
                        ((uint8_t*)PHYSICAL_TO_VIRTUAL(rx_descriptor[i].low_mem))[6], // descriptor's
                        ((uint8_t*)PHYSICAL_TO_VIRTUAL(rx_descriptor[i].low_mem))[7]  // buffer!
                      );
        }

        // set this descriptor free to use again
        rx_descriptor[i].cmd_status = 0x80000000 | (RX_BUFFER_SIZE & 0x3FFF);
      }
  } else
  if( status & RTL81XX_RX_ERR) {
      // Error on Recv Packet
  } else
  if( status & RTL81XX_TX_OK) {
    // packet trasmited
  } else
  if( status & RTL81XX_TX_ERR) {
    // packet trasmit error
  } else
  if( status & RTL81XX_RX_UNAVLB) {
      printf("RTL81xx: RX descriptors unavailable (%d packet loss)\n", inw(rtl81xx_io_base + 0x4C) & 0xFFF); // 24-bits packet loss count
      printf("0x%x%x\n", inl(rtl81xx_io_base + 0xE8), inl(rtl81xx_io_base + 0xE4));
      rtl81xx_stop();
  } else
  if( status & RTL81XX_LINK_CHG) {
      rtl81xx_link_up = rtl81xx_link_is_up();
      printf("RTL81xx: link ");
      if(rtl81xx_link_up) {
        rtl81xx_start();
        printf("up!\n");
      }
      else {
        rtl81xx_stop();
        printf("down!\n");
      }

  } else
  if( status & RTL81XX_RX_OVERFLOW) {
      printf("RTL81xx: RX overflow (%d packet loss)\n", inw(rtl81xx_io_base + 0x4C) & 0xFFF); // 24-bits packet loss count
  } else
  if( status & RTL81XX_TX_UNAVLB) {
      printf("RTL81xx: TX descriptors unavailable\n"); // 24-bits packet loss count
      rtl81xx_stop();
  } else
  if( status & RTL81XX_SOFT_INT) {
      // software interrupt
  } else
  if( status & RTL81XX_TIMEOUT) {
      printf("RTL81xx: timeout.\n");
      rtl81xx_stop();
  } else
  if( status & RTL81XX_SYS_ERR) {
      printf("RTL81xx: has a hardware error. (stoping Rx/Tx)\n");
      rtl81xx_stop();
  } else {
      printf("RTL81xx: unknown reason Interrupt.\n");
  }

  // clear the Interrupt Bit Flag (writing itself!)
  // [datasheet note: "Writing 1 to any bit in the ISR will reset that bit." ]
  outw(rtl81xx_io_base + 0x3E, status);

  pic_acknowledge(rtl81xx_irq);
  __asm__ __volatile__("add $12, %esp\n");
  __asm__ __volatile__("leave\n");
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
  printf("found!\n");

  //get IO base addr
  rtl81xx_io_base = pci_read(rtl81xx_bus, rtl81xx_dev, rtl81xx_function, PCI_BAR0);
  rtl81xx_io_base = rtl81xx_io_base & 0xFFFC; // 4-byte aligned

  rtl81xx_irq = pci_read(rtl81xx_bus, rtl81xx_dev, rtl81xx_function, 0x3C) & 0xFF;
  irq_install(rtl81xx_irq, rtl81xx_handler);

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
      rx_descriptor[i].low_mem = (uint32_t)VIRTUAL_TO_PHYSICAL((uint32_t)&rx_buffer[i]);
  }
  rx_descriptor[i-1].cmd_status |= 0x40000000; // set End Of Ring flag on last entry

  // setup tx descriptors
  for( i = 0; i < TX_DESCRIPTORS; i++) {
      tx_descriptor[i].cmd_status = 0x80000000 | (TX_BUFFER_SIZE & 0x3FFF);
      tx_descriptor[i].hi_mem = 0;
      tx_descriptor[i].low_mem = (uint32_t)VIRTUAL_TO_PHYSICAL((uint32_t)&tx_buffer[i]);
  }
  tx_descriptor[i-1].cmd_status |= 0x40000000; // set End Of Ring flag on last entry

  rtl81xx_unlock();   // allow modify config registers

  outb(rtl81xx_io_base + 0x3C, 0xFF); // enable all interrupts
  outb(rtl81xx_io_base + 0x3D, 0xFF);

  // about RX packets
  outw(rtl81xx_io_base + 0xDA, 8192);   // max  tx packet size
  outl(rtl81xx_io_base + 0x44, 0xE70F); // accept anything (test)

  // about TX packets
  outw(rtl81xx_io_base + 0xEC, 0x00);   // max  tx packet size  in 32-bytes unit
  outl(rtl81xx_io_base + 0x41, 0x7);   // DMA size Unlimited

  // setup LED mode
  outb(rtl81xx_io_base + 0x52, 0xC0); // configure LED mode 4 (rx/tx activity)

  outl(rtl81xx_io_base + 0xE4, (uint32_t)VIRTUAL_TO_PHYSICAL(rx_descriptor));
  outl(rtl81xx_io_base + 0xE8, 0x00000000);
  outl(rtl81xx_io_base + 0x20, (uint32_t)VIRTUAL_TO_PHYSICAL(tx_descriptor));
  outl(rtl81xx_io_base + 0x24, 0x00000000);

  outb(rtl81xx_io_base + 0x37, 0x0C);  // configure Command. enable RX/TX
  rtl81xx_lock();


  printf("\tio_base=0x%x, irq=%d\n", rtl81xx_io_base, rtl81xx_irq);
  printf("\tMAC=%x:%x:%x:%x:%x:%x\n", rtl81xx_MAC[0],rtl81xx_MAC[1],
                                      rtl81xx_MAC[2],rtl81xx_MAC[3],
                                      rtl81xx_MAC[4],rtl81xx_MAC[5]);
}

void
rtl81xx_reset() {
    outb(rtl81xx_io_base + 0x37, 0x10);
    while(inb(rtl81xx_io_base + 0x37) & 0x10);
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
    return rtl81xx_link_status() & RTL81XX_LINK_OK;
}

uint8_t
rtl81xx_link_status() {
    return inb(rtl81xx_io_base + 0x6C);
}

void
rtl81xx_read_packet(uint8_t* payload, uint32_t* payload_size) {

}

void
rtl81xx_write_packet(uint8_t* payload, uint32_t payload_size) {

}
