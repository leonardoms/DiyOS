#ifndef _RTL81XX_H_
#define _RTL81XX_H_

#include <pci.h>
#include <x86/x86.h>
#include <small.h>

struct _rtl81xx_id {
  uint16_t  vendor;
  uint16_t  device;
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

#define RTL81XX_RX_OK         0x0001
#define RTL81XX_RX_ERR        0x0002
#define RTL81XX_TX_OK         0x0004
#define RTL81XX_TX_ERR        0x0008
#define RTL81XX_RX_UNAVLB     0x0010
#define RTL81XX_LINK_CHG      0x0020
#define RTL81XX_RX_OVERFLOW   0x0040
#define RTL81XX_TX_UNAVLB     0x0080
#define RTL81XX_SOFT_INT      0x0100
#define RTL81XX_TIMEOUT       0x4000
#define RTL81XX_SYS_ERR       0x8000

void
setup_rtl81xx();

// start Rx/Tx
void
rtl81xx_start();

// stop Rx/Tx
void
rtl81xx_stop();

// soft reset
void
rtl81xx_reset();

#endif
