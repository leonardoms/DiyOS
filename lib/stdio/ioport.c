#include <ctypes.h>

void enable(void) {
    __asm__ __volatile__("sti");
}

void disable(void) {
    __asm__ __volatile__("cli");
}

void outportb(uint32_t port, uint8_t value){
	   __asm__ __volatile__("outb %%al,%%dx"::"d" (port), "a" (value));
};

void outportw(uint32_t port, uint16_t value){
	   __asm__ __volatile__("outw %%ax,%%dx"::"d" (port), "a" (value));
};


void outportl(uint32_t port, uint32_t value){
	   __asm__ __volatile__("outl %%eax,%%dx"::"d" (port), "a" (value));
};


unsigned char inportb(uint32_t port){
	uint8_t ret;

	__asm__ __volatile__("inb %%dx,%%al":"=a" (ret):"d"(port));

	return ret;
};

uint16_t inportw(uint32_t port){
	uint16_t ret;

	__asm__ __volatile__("inw %%dx,%%ax":"=a" (ret):"d"(port));

	return ret;
};

uint32_t inportl(uint32_t port){
	uint32_t ret;

	__asm__ __volatile__("inl %%dx,%%eax":"=a" (ret):"d"(port));

	return ret;
};
