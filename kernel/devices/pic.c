// Programable Interrupt Controler
#include <kernel.h>
#include <stdio.h>

#define PIC_MASTER_CTRL 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CTRL  0xA0
#define PIC_SLAVE_DATA  0xA1


void
pic(void) {

    // ICW1
    outportb(PIC_MASTER_CTRL, 0x11);  // init master PIC
    outportb(PIC_SLAVE_CTRL, 0x11);   // init slave PIC
    // ICW2
    outportb(PIC_MASTER_DATA, 0x20);  // IRQ 0..7 remaped to 0x20..0x27
    outportb(PIC_SLAVE_DATA, 0x28);   // IRQ 8..15 remaped to 0x28..0x37
    // ICW3
    outportb(PIC_MASTER_DATA, 0x04);  // set as Master
    outportb(PIC_SLAVE_DATA, 0x02);   // set as Slave
    // ICW4
    outportb(PIC_MASTER_DATA, 0x01);  // set x86 mode
    outportb(PIC_SLAVE_DATA, 0x01);   // set x86 mode

    outportb(PIC_MASTER_DATA, 0xFF);  // all interrupts disabled
    outportb(PIC_SLAVE_DATA, 0xFF);

    __asm__ __volatile__("nop");
}

static uint16_t ocw1 = 0xFFFB;

void irq_enable(uint8_t irq)
{
	ocw1 &= (uint16_t)~((1 << irq));

	if (irq < 8)
		outportb(PIC_MASTER_DATA, (uint8_t)(ocw1 & 0xFF));
	else
		outportb(PIC_SLAVE_DATA, (uint8_t)(ocw1 >> 8));
}

void pic_acknowledge(uint8_t irq)
{
  if ( irq > 7)
    outportb(PIC_SLAVE_CTRL, 0x20);

  outportb(PIC_MASTER_CTRL, 0x20);
}
