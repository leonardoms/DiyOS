
#include <kernel.h>

#define SECTOR_COUNT  2
#define LBA_LOW       3
#define LBA_MID       4
#define LBA_HIG       5

uint8_t ide_bus, ide_dev, ide_fun;

uint16_t ide_primary_io_base, ide_primary_ctrl, ide_secondary_io_base, ide_secondary_ctrl, ide_bus_master_base;

void
ide(void) {

  if( !pci_find_class(0x1, 0x1, &ide_bus, &ide_dev, &ide_fun) ) // find mass storage / scsi bus
    return;

  if( (ide_primary_io_base = pci_read(ide_bus, ide_dev, ide_fun, PCI_BAR0)) <= 1 )
    ide_primary_io_base = 0x1F0;

  if( (ide_primary_ctrl = pci_read(ide_bus, ide_dev, ide_fun, PCI_BAR1)) <= 1 )
    ide_primary_ctrl = 0x3F6;

  if( (ide_secondary_io_base = pci_read(ide_bus, ide_dev, ide_fun, PCI_BAR2)) <= 1 )
    ide_secondary_io_base = 0x170;

  if( (ide_secondary_ctrl = pci_read(ide_bus, ide_dev, ide_fun, PCI_BAR3)) <= 1 )
    ide_secondary_ctrl = 0x376;

  ide_bus_master_base = pci_read(ide_bus, ide_dev, ide_fun, PCI_BAR4);

  uint8_t i;
  printf("\nPRI: ");
  for( i = 2; i < 6; i++)
    printf("0x%x ", inportb(ide_primary_io_base + i) );

  printf("\nSEC: ");
  for( i = 2; i < 6; i++)
    printf("0x%x ", inportb(ide_secondary_io_base + i) );

}
