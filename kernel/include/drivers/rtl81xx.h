#ifndef _RTL81XX_H_
#define _RTL81XX_H_

#include <pci.h>
#include <x86/x86.h>
#include <small.h>

const struct _rtl81xx_id {
  uint16_t  vendor;
  uint16_t  device;
} rtl81xx_id[] = {
  { 0x10EC, 0x8168 }, // RTL-8029
  { 0, 0}
};

#define RX_DESCRIPTORS    64    // can be up to 1024
#define RX_BUFFER_SIZE    1024

#define TX_DESCRIPTORS    64    // can be up to 1024
#define TX_BUFFER_SIZE    1024

struct x_descriptor {
    uint32_t cmd_status;
    uint32_t reserved;
    uint32_t low_mem; // 0 .. 32
    uint32_t hi_mem;  // 33 .. 64 (for 64-bits system)
};

#endif
