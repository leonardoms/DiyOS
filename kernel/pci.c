#include <small.h>


#define PCI_CONFIG  0xCF8
#define PCI_DATA    0xCFC

#define PCI_VENDOR_DEVICE   0x00
#define PCI_BAR0    0x10
#define PCI_BAR1    PCI_BAR0 + 4
#define PCI_BAR2    PCI_BAR1 + 4
#define PCI_BAR3    PCI_BAR2 + 4
#define PCI_BAR4    PCI_BAR3 + 4
#define PCI_BAR5    PCI_BAR4 + 4

uint32_t
pci_read(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset) {
    uint32_t reg = 0x80000000;

    reg |= (bus & 0xFF) << 16;
    reg |= (device & 0x1F) << 11;
    reg |= (function & 0x7) << 8;
    reg |= (offset & 0xFF) & 0xFC;

    outl(PCI_CONFIG, reg );

    return inl(PCI_DATA);
}

void
pci_write(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset, uint32_t data) {
    uint32_t reg = 0x80000000;

    reg |= (bus & 0xFF) << 16;
    reg |= (device & 0x1F) << 11;
    reg |= (function & 0x7) << 8;
    reg |= offset & 0xFC;

    outl(PCI_CONFIG, reg );
    outl(PCI_DATA, data);
}

void
pci_test() {
    uint32_t vend_dev, b, d, f;
    printf("Devices over PCI:\n");
    // print all devices
    for(b = 0; b < 256; b++)
      for(d = 0; d < 32; d++)
        for(f = 0; f < 8; f++){
          vend_dev = pci_read(b,d,f,PCI_VENDOR_DEVICE);
          if(vend_dev != 0xFFFFFFFF)
            printf("[%d:%d.%d] %x:%x\n", b, d, f, vend_dev & 0xFFFF, vend_dev >> 16);
        }
}
