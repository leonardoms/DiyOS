#include <drivers/rtl8139.h>

struct ether {
  uint8_t      dst[6];
  uint8_t      src[6];
  uint16_t  type;
} __attribute__ ((packed));

struct arp {
  struct    ether;
  uint16_t  hw;
  uint16_t  proto;
  uint8_t   hw_len;
  uint8_t   proto_len;
  uint16_t  op_code;
  uint8_t   src_hw[6];       // for ether TCP
  uint8_t   src_proto[4]; // for IP
  uint8_t   dst_hw[6];       // for ether TCP
  uint8_t   dst_proto[4]; // for IP
}__attribute__ ((packed));

void
pkt_process(uint8_t* pkt, uint16_t size) {
  struct ether* eth;
  struct arp*   _arp;

  eth = (struct ether*)pkt;

  if(eth->type == 0x0608) { // ARP 0x0806
      _arp = (struct arp*)pkt;
      printf("ARP from %d.%d.%d.%d (%x:%x:%x:%x:%x:%x): who has %d.%d.%d.%d ?\n",
              _arp->src_proto[0], _arp->src_proto[1], _arp->src_proto[2], _arp->src_proto[3],
              _arp->src_hw[0], _arp->src_hw[1], _arp->src_hw[2], _arp->src_hw[3], _arp->src_hw[4], _arp->src_hw[5],
              _arp->dst_proto[0], _arp->dst_proto[1], _arp->dst_proto[2], _arp->dst_proto[3]);
  } else
  if(eth->type == 0xdd86) {
      printf("IPv6 from %x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x (%d bytes)\n",
              eth->src[0], eth->src[1], eth->src[2], eth->src[3], eth->src[4], eth->src[5],
              eth->dst[0], eth->dst[1], eth->dst[2], eth->dst[3], eth->dst[4], eth->dst[5], size );
  } else
  if(eth->type == 0x0008) {
      printf("IPv4 from %x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x (%d bytes)\n",
              eth->src[0], eth->src[1], eth->src[2], eth->src[3], eth->src[4], eth->src[5],
              eth->dst[0], eth->dst[1], eth->dst[2], eth->dst[3], eth->dst[4], eth->dst[5, size] );
  } else  {
      printf("Ethernet (0x%x) from %x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x (%d bytes)\n",
              eth->type, eth->src[0], eth->src[1], eth->src[2], eth->src[3], eth->src[4], eth->src[5],
              eth->dst[0], eth->dst[1], eth->dst[2], eth->dst[3], eth->dst[4], eth->dst[5], size );
  }

}

struct _rtl8139_id {
  uint16_t  vendor;
  uint16_t  device;
} rtl8139_id[] = {
  { 0x10EC, 0x8139 }, // RTL-8139
  { 0, 0}
};

#define RTL8139_TX_LEN    0x600
#define RTL8139_TX_COUNT  4
#define RTL8139_TX_TOTAL_LEN               (RTL8139_TX_LEN * RTL8139_TX_COUNT)
#define RTL8139_TX_BUFFER(base,i)          (base+i*RTL8139_TX_LEN)
#define RTL8139_TX_BUFFER_PHYS(base,i)     VIRTUAL_TO_PHYSICAL(base+i*RTL8139_TX_LEN)

uint16_t  rtl8139_io_base;
uint8_t   rtl8139_MAC[6];
uint8_t   rtl8139_bus, rtl8139_dev, rtl8139_function;
uint8_t   rtl8139_irq, rtl8139_link_up, rtl8139_tx_in_use;
uint8_t   rtl8139_rx_buffer[65536], rtl8139_tx_buffer[RTL8139_TX_TOTAL_LEN];
uint32_t  rtl8139_rx_buffer_size, rtl8139_rx_cur;



uint8_t   arp_test[42] = {  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xde, 0xad, 0xbe, 0xef, 0xaa, 0xbb,
                            0x08, 0x06, 0x00, 0x01, 0x08, 0x00,
                            0x06, 0x04, 0x00, 0x01, 0xfe, 0xb0,
                            0xe8, 0x99, 0x0f, 0xd2, 0x2a, 0x2a, // sender: 42.42.0.15  (fe:b0:e8:99:0f:d2)
                            0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, // target: 192.168.0.1 (00:00:00:00:00:00)
                            0x00, 0x00, 0xc0, 0xa7, 0x00, 0x01 };

uint32_t tx_arp_desc[4];

static void
rtl8139_handler() {

  uint16_t  status, bptr, baddr, offset, pkt_size, rx_size;
  uint32_t  i, *rx, pkt_status;
  uint8_t   *pkt_content;

  status = inw(rtl8139_io_base + RTL8139_ISR);

  outw(rtl8139_io_base + RTL8139_ISR, status);

  if(status & 0x0001) {
      while( (inb(rtl8139_io_base + RTL8139_CMD) & 0x01) == 0 ) // is the buffer empty?
      {
        if(rtl8139_rx_cur > rtl8139_rx_buffer_size)
          rtl8139_rx_cur = rtl8139_rx_cur % rtl8139_rx_buffer_size;

          rx = (uint32_t*)rtl8139_rx_buffer + rtl8139_rx_cur;
          pkt_content = (uint8_t*)(rtl8139_rx_buffer + rtl8139_rx_cur + 4);

          rx_size = *rx >> 16;

          pkt_size = rx_size - 4;


          if(pkt_size > 0 && pkt_size < 1500 /* && (pkt_status == 1) */) {
             pkt_process(pkt_content, pkt_size);
          }
          rtl8139_rx_cur = ( rtl8139_rx_cur + rx_size + 4 + 3 ) & ~3;
          outw(rtl8139_io_base + RTL8139_CAPR, rtl8139_rx_cur - 16);

      }
  } else
  if(status & 0x0004) { // TxOK
      printf("TxOK ");
      if(rtl8139_tx_in_use > 0)
        rtl8139_tx_in_use--;
  } else printf("rtl8139: 0x%x\n", status);

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

  rtl8139_rx_buffer_size = 0xFFFF;
  rtl8139_rx_cur = 0;


  // power on
  outb(rtl8139_io_base + RTL8139_CONFIG1, 0x0);

  outl(rtl8139_io_base + RTL8139_MAR, 0xFFFFFFFF);
  outl(rtl8139_io_base + RTL8139_MAR + 4, 0xFFFFFFFF);

  // softreset
  outb(rtl8139_io_base + RTL8139_CMD, 0x10);
  while( (inb(rtl8139_io_base + RTL8139_CMD) & 0x10) != 0) { }

  outl(rtl8139_io_base + RTL8139_RBSTART, (uint32_t)VIRTUAL_TO_PHYSICAL(&rtl8139_rx_buffer) );

  outw(rtl8139_io_base + RTL8139_CAPR, 0xFFF0 ); //CAPR

  for(i = 0; i < RTL8139_TX_COUNT; i++) {
    outl(rtl8139_io_base + RTL8139_TSAD0 + i*4, RTL8139_TX_BUFFER_PHYS(rtl8139_tx_buffer,i));
  }
  rtl8139_tx_in_use = 0;
  outl(rtl8139_io_base + RTL8139_TCR, 1024);

  for(i = 0; i < 6; i++)
    rtl8139_MAC[i] = inb(rtl8139_io_base + RTL8139_MAC + i);

  printf("\tio_base=0x%x, irq=%d\n", rtl8139_io_base, rtl8139_irq);
  printf("\tMAC=%x:%x:%x:%x:%x:%x\n", rtl8139_MAC[0],rtl8139_MAC[1],
                                      rtl8139_MAC[2],rtl8139_MAC[3],
                                      rtl8139_MAC[4],rtl8139_MAC[5]);

  outl(rtl8139_io_base + RTL8139_RCR, 0xF | (1 << 7));//0xFCBF); // receive ALL packets

  irq_install(rtl8139_irq, rtl8139_handler);

  // Start rx/tx
  outb(rtl8139_io_base + RTL8139_CMD, 0x0C);
  outw(rtl8139_io_base + RTL8139_IMR, 0x7F);

  rtl8139_tx(arp_test,42);

}


uint8_t
rtl8139_tx(uint8_t* data, uint16_t size) {
  static int d = 0;

  if(rtl8139_tx_in_use > RTL8139_TX_COUNT) {
    return 0;
  }

  rtl8139_tx_in_use++;
  memcpy(data,RTL8139_TX_BUFFER(rtl8139_tx_buffer,d),size);

  outl(rtl8139_io_base + RTL8139_TSD0 + d*4, size);

  d++;
}
