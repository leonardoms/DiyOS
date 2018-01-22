#ifndef _PIC_H_
#define _PIC_H_

#include "io.h"

#define IRQ_BASE  0x20

void setup_pic();
void irq_enable(uint8_t irq);
void pic_acknowledge(uint8_t irq);

#endif
