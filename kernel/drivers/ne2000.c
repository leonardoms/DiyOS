#include <pci.h>
#include <x86/x86.h>
#include <small.h>

const struct _ne2000_id {
  uint16_t  vendor;
  uint16_t  device;
} ne2000_id[] = {
  { 0x1050, 0x0940 },
  { 0x1050, 0x5A5A },
  { 0x10BD, 0x0E34 },
  { 0x10EC, 0x8029 }, // RTL-8029
  { 0x1106, 0x0926 },
  { 0x11F6, 0x1401 },
  { 0x12C3, 0x0058 },
  { 0x12C3, 0x5598 },
  { 0x4A14, 0x5000 },
  { 0x8C4A, 0x1980 },
  { 0x8E2E, 0x3000 },
  { 0, 0}
};

// reference: http://www.ethernut.de/pdf/8019asds.pdf

uint16_t  ne2000_io_base;
uint8_t   ne2000_MAC[6];
uint8_t   ne2000_bus, ne2000_dev, ne2000_function;

void
ne2000_handler() {
  printf(".");
}
IRQN(10,ne2000_handler);

void
setup_ne2000() {
  uint32_t i;
  uint8_t ne2000_present;

  printf("ne2000 compatible network card: ");

  ne2000_present = 0;
  i = 0;
  while(ne2000_id[i].vendor) {
    if(pci_find(ne2000_id[i].vendor, ne2000_id[i].device, &ne2000_bus,
                &ne2000_dev, &ne2000_function)) {
        ne2000_present = 1;
        break;
      }
    i++;
  }

  if(ne2000_present == 0) {
    printf("not found\n");
    return;
  }

  // okay, device was found

  //get IO base addr
  ne2000_io_base = pci_read(ne2000_bus, ne2000_dev, ne2000_function, PCI_BAR0);
  ne2000_io_base = ne2000_io_base & 0xFFFC; // 4-byte aligned

  pci_write(ne2000_bus, ne2000_dev, ne2000_function, 0x3c, 0x10A);

  outb(ne2000_io_base + 0x1F, 0xFF); // reset
  while(inb(ne2000_io_base + 0x7) == 0);

  ne2000_stop();

  printf("\n\tio_base=0x%x, ", ne2000_io_base);

  ne2000_page(0);
  outb(ne2000_io_base + 0xE, 0x89); // DMA word mode; Loopback Normal.
  outb(ne2000_io_base + 0x7, 0xFF); // datasheet: "must be cleared after power up" ... "are cleared by writing a '1'"
  outb(ne2000_io_base + 0xC, 0xFF); // accept any packet (for test!)

  for(i = 0; i < 6; i++)
    ne2000_MAC[i] = inb(ne2000_io_base + 0x10);

  printf("MAC=%x:%x:%x:%x:%x:%x\n", ne2000_MAC[0],ne2000_MAC[1],ne2000_MAC[2],
                        ne2000_MAC[3],ne2000_MAC[4],ne2000_MAC[5]);  // MAC[0]

  ne2000_set_MAC(ne2000_MAC);
  outb(ne2000_io_base, 0x8);  // allow receive remote packets!

  irq_install(10,irq10);

  ne2000_start();
}

void
ne2000_set_MAC(char mac[6]) {
  ne2000_page(1);
  outb(ne2000_io_base + 0x1, mac[0]);
  outb(ne2000_io_base + 0x2, mac[1]);
  outb(ne2000_io_base + 0x3, mac[2]);
  outb(ne2000_io_base + 0x4, mac[3]);
  outb(ne2000_io_base + 0x5, mac[4]);
  outb(ne2000_io_base + 0x6, mac[5]);
}

// change to configuration 'page' number
void
ne2000_page(uint8_t page) {
  uint8_t flags;
  flags = inb(ne2000_io_base + 0);
  outb(ne2000_io_base + 0, ((page & 0x3) << 5) | (flags & 0x3F));
}

void
ne2000_start() {
  outb(ne2000_io_base + 0, (inb(ne2000_io_base + 0) & 0xFC) | 2); // preserve other flags
}

void
ne2000_stop() {
  outb(ne2000_io_base + 0, (inb(ne2000_io_base + 0) & 0xFC) | 1);  // preserve other flags
}
