#ifndef _IO_H_
#define _IO_H_

#include "ctypes.h"

void outportb(uint32_t port, uint8_t value);
uint8_t inportb(uint32_t port);
void outportw(uint32_t port, uint16_t value);
uint16_t inportw(uint32_t port);
void outportl(uint32_t port, uint32_t value);
uint32_t inportl(uint32_t port);

#define outb(a,b)  outportb(a,b)
#define inb(a)  inportb(a)
#define outw(a,b)  outportw(a,b)
#define inw(a)  inportw(a)
#define outl(a,b)  outportl(a,b)
#define inl(a)  inportl(a)

void enable();
void disable();

#endif
