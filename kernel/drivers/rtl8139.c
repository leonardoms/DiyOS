#include <drivers/rtl8139.h>

struct _rtl8139_id {
  uint16_t  vendor;
  uint16_t  device;
} rtl8139_id[] = {
  { 0x10EC, 0x8139 }, // RTL-8139
  { 0, 0}
};

// (1).  Initialization procedure
//       1. Enable Transmit/Receive(RE/TE in CommandRegister)
//       2. configure TCR/RCR.
//       3. Enable IMR.
// (2).  Transmit reset and Receive reset can be done individually.

uint16_t  rtl8139_io_base;
uint8_t   rtl8139_MAC[6];
uint8_t   rtl8139_bus, rtl8139_dev, rtl8139_function;
uint8_t   rtl8139_irq, rtl8139_link_up;
uint8_t   *rtl8139_rx_buffer;

uint8_t   arp_test[42] = {  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xde, 0xad, 0xbe, 0xef, 0xaa, 0xbb,
                            0x08, 0x06, 0x00, 0x01, 0x08, 0x00,
                            0x06, 0x04, 0x00, 0x01, 0xfe, 0xb0,
                            0xe8, 0x99, 0x0f, 0xd2, 0x2a, 0x2a, // sender: 42.42.0.15  (fe:b0:e8:99:0f:d2)
                            0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, // target: 192.168.0.1 (00:00:00:00:00:00)
                            0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01 };

uint32_t tx_arp_desc[4];

static void
rtl8139_handler() {

  uint16_t status;
  uint32_t i;

  status = inw(rtl8139_io_base + RTL8139_ISR);

  printf("rtl8139: 0x%x\n", status);

  outw(rtl8139_io_base + RTL8139_ISR, status);

  pic_acknowledge(rtl8139_irq);
  __asm__ __volatile__("add $12, %esp\n");
  __asm__ __volatile__("leave\n");
  __asm__ __volatile__("iret\n");

}

void setup_rtl8139() {
  uint32_t  i, pci_cmd;
  uint8_t   rtl8139_present;

  printf("Realtek 8139 network card: ");

  rtl8139_present = 0;
  i = 0;
  while(rtl8139_id[i].vendor) {
    if(pci_find(rtl8139_id[i].vendor, rtl8139_id[i].device, &rtl8139_bus,
                &rtl8139_dev, &rtl8139_function)) {
        rtl8139_present = 1;
        break;
      }
    i++;
  }

  if(rtl8139_present == 0) {
    printf("not found\n");
    return;
  }
  // okay, device was found
  printf("found!\n");

  //get IO base addr
  rtl8139_io_base = pci_read(rtl8139_bus, rtl8139_dev, rtl8139_function, PCI_BAR0);
  rtl8139_io_base = rtl8139_io_base & 0xFFFC; // 4-byte aligned

  rtl8139_irq = pci_read(rtl8139_bus, rtl8139_dev, rtl8139_function, 0x3C) & 0xFF;

  pci_cmd = pci_read(rtl8139_bus, rtl8139_dev, rtl8139_function, 0x04); // read Command Register from PCI
  pci_write(rtl8139_bus, rtl8139_dev, rtl8139_function, 0x04, pci_cmd | 4); // Busmastering is ON!

//  rtl8139_rx_buffer = kmalloc(0xFFFF); // 64KB
  rtl8139_rx_buffer = (uint8_t*)( 0x100000 - 0xFFFF );
//  if( !rtl8139_rx_buffer ) {
//    printf("RTL8139: failed to alloc resources!\n");
//    return;
//  }

  // power on
  outb(rtl8139_io_base + RTL8139_CONFIG1, 0x0);

  outl(rtl8139_io_base + RTL8139_MAR, 0xFFFFFFFF);
  outl(rtl8139_io_base + RTL8139_MAR + 4, 0xFFFFFFFF);

  // softreset
  outb(rtl8139_io_base + RTL8139_CMD, 0x10);
  while( (inb(rtl8139_io_base + RTL8139_CMD) & 0x10) != 0) { }

  outl(rtl8139_io_base + RTL8139_RBSTART, (uint32_t)rtl8139_rx_buffer);//VIRTUAL_TO_PHYSICAL(rtl8139_rx_buffer) );

  outw(rtl8139_io_base + RTL8139_RX_CURRENT, 0xFFF0 ); //CAPR

  for(i = 0; i < 6; i++)
    rtl8139_MAC[i] = inb(rtl8139_io_base + RTL8139_MAC + i);

  printf("\tio_base=0x%x, irq=%d\n", rtl8139_io_base, rtl8139_irq);
  printf("\tMAC=%x:%x:%x:%x:%x:%x\n", rtl8139_MAC[0],rtl8139_MAC[1],
                                      rtl8139_MAC[2],rtl8139_MAC[3],
                                      rtl8139_MAC[4],rtl8139_MAC[5]);

  outl(rtl8139_io_base + RTL8139_RCR, 0xFCBF); // receive ALL packets

  irq_install(rtl8139_irq, rtl8139_handler);

  // Start rx/tx
  outb(rtl8139_io_base + RTL8139_CMD, 0x0C);
  outw(rtl8139_io_base + RTL8139_IMR, 0x5);

  tx_arp_desc[0] = 0x8001002A;
  tx_arp_desc[1] = 0;
  tx_arp_desc[2] = (uint32_t)VIRTUAL_TO_PHYSICAL(arp_test);
  tx_arp_desc[3] = 0;

  //while(1)
    outw(rtl8139_io_base + 0x20, (uint32_t)VIRTUAL_TO_PHYSICAL(tx_arp_desc));
}

//FF + 700 + E000 + 1000 +
