#include "ctypes.h"

void enable() {
    __asm__ __volatile__("sti");
}

void disable() {
    __asm__ __volatile__("cli");
}

void outportb(uint32_t port, uint8_t value){
	   __asm__ __volatile__("outb %%al,%%dx"::"d" (port), "a" (value));
};

unsigned char inportb(uint32_t port){
	uint8_t ret;

	__asm__ __volatile__("inb %%dx,%%al":"=a" (ret):"d"(port));

	return ret;
};
