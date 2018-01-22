
#include <pci.h>
#include <small.h>
#include <io.h>

#define BOCHS_VBE_VENDOR  0x1234
#define BOCHS_VBE_DEVICE  0x1111

#define VBE_DISPI_IOPORT_INDEX  0x1CE
#define VBE_DISPI_IOPORT_DATA   0x1CF

#define VBE_DISPI_INDEX_ID          0
#define VBE_DISPI_INDEX_XRES        1
#define VBE_DISPI_INDEX_YRES        2
#define VBE_DISPI_INDEX_BPP         3
#define VBE_DISPI_INDEX_ENABLE      4
#define VBE_DISPI_INDEX_BANK        5
#define VBE_DISPI_INDEX_VIRT_WIDTH  6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET    8
#define VBE_DISPI_INDEX_Y_OFFSET    9

// flags for VBE_DISPI_INDEX_ENABLE
#define VBE_DISPI_DISABLED          0x00
#define VBE_DISPI_ENABLED           0x01
#define VBE_DISPI_LFB_ENABLED       0x40
#define VBE_DISPI_NOCLEARMEM        0x80

uint8_t bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function;
uint8_t* bochs_vbe_fb;

void
setup_bochs_vbe() {
    uint32_t aux;

    printf("Bochs VGA: ");
    if(!pci_find(BOCHS_VBE_VENDOR, BOCHS_VBE_DEVICE, &bochs_vbe_bus,
                &bochs_vbe_dev, &bochs_vbe_function)) {
      printf("not found\n");
      return;
    }
    printf("found @ pci/%d:%d.%d\n", bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function);

    aux = pci_read(bochs_vbe_bus, bochs_vbe_dev, bochs_vbe_function, PCI_BAR0);
    bochs_vbe_fb = (uint8_t*)(aux & 0xFFFFFFF0);

    bochs_vbe_display(800,600,24); // set default resolution 800x600 (24-bits)
}

void
bochs_vbe_write(uint16_t index, uint16_t value) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, value);
}

uint16_t
bochs_vbe_read(uint16_t index) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return inw(VBE_DISPI_IOPORT_DATA);
}

void
bochs_vbe_disable() {
  bochs_vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

void
bochs_vbe_enable() {
  bochs_vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

void
bochs_vbe_display(uint16_t width, uint16_t height, uint16_t depth) {
  bochs_vbe_disable();
  bochs_vbe_write(VBE_DISPI_INDEX_XRES, width);
  bochs_vbe_write(VBE_DISPI_INDEX_YRES, height);
  bochs_vbe_write(VBE_DISPI_INDEX_BPP, depth);
  bochs_vbe_enable();
}

uint16_t
bochs_vbe_version() {
  return 0xB0C5; // TODO
}
