#include "ctypes.h"

void enable() {
    asm volatile ("sti");
}

void disable() {
    asm volatile ("cli");
}

void outportb(uint32_t port, uint8_t value){
	   asm volatile ("outb %%al,%%dx"::"d" (port), "a" (value));
}

unsigned char inportb(uint32_t port){
	uint8_t ret;

	asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));

	return ret;
}
