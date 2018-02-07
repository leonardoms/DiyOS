#ifndef _RTL81XX_H_
#define _RTL81XX_H_

#include <pci.h>
#include <x86/x86.h>
#include <small.h>

#define RX_DESCRIPTORS    1024    // can be up to 1024
#define RX_BUFFER_SIZE    128

#define TX_DESCRIPTORS    1024    // can be up to 1024
#define TX_BUFFER_SIZE    128

struct x_descriptor {
    uint32_t cmd_status;
    uint32_t reserved;
    uint32_t low_mem; // 0 .. 32
    uint32_t hi_mem;  // 33 .. 64 (for 64-bits system)
} __attribute__ ( (__packed__, aligned (256)) );

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


// flags for rtl81xx_link_status()
#define RTL81XX_LINK_FULL       0x01
#define RTL81XX_LINK_OK         0x02
#define RTL81XX_LINK_10M        0x04
#define RTL81XX_LINK_100M       0x08
#define RTL81XX_LINK_1000M      0x10
#define RTL81XX_LINK_RX         0x20
#define RTL81XX_LINK_TX         0x40

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

uint8_t
rtl81xx_link_status();

uint8_t
rtl81xx_link_is_up();

#endif
