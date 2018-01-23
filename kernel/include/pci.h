#ifndef _PCI_H_
#define _PCI_H_

#include <small.h>
#include <io.h>

// config Offsets
#define PCI_VENDOR_DEVICE   0x00
#define PCI_BAR0    0x10
#define PCI_BAR1    PCI_BAR0 + 4
#define PCI_BAR2    PCI_BAR1 + 4
#define PCI_BAR3    PCI_BAR2 + 4
#define PCI_BAR4    PCI_BAR3 + 4
#define PCI_BAR5    PCI_BAR4 + 4

uint8_t
pci_find(uint32_t vendor, uint32_t device, uint8_t* bus,  uint8_t* dev, uint8_t* function);

uint32_t
pci_read(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset);

void
pci_write(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset, uint32_t data);

#endif
