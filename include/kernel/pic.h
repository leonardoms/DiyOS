#ifndef _PIC_H_
#define _PIC_H_

#include <ctypes.h>

#define IRQ_BASE  0x20

void pic(void);
void irq_enable(uint8_t irq);
void pic_acknowledge(uint8_t irq);

#endif
