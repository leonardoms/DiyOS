#ifndef _RTL8139_H_
#define _RTL8139_H_

#include <pci.h>
#include <x86/x86.h>
#include <small.h>


#define RTL8139_MAC     0x00
#define RTL8139_MAR     0x08
#define RTL8139_RBSTART 0x30
#define RTL8139_CMD     0x37
#define RTL8139_CAPR    0x38
#define RTL8139_IMR     0x3C
#define RTL8139_ISR     0x3E
#define RTL8139_RCR     0x44
#define RTL8139_CONFIG1 0x52

void
setup_rtl8139();

#endif
