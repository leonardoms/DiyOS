#ifndef _IO_H_
#define _IO_H_

#include "ctypes.h"

void outportb(uint32_t port, uint8_t value);
unsigned char inportb(uint32_t port);

#define outb(a,b)  outportb(a,b)
#define inb(a)  inportb(a)

void enable();
void disable();

#endif
