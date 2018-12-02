#ifndef _BOCHS_VBE_H_
#define _BOCHS_VBE_H_


#include <kernel.h>

void
setup_bochs_vbe();

/* set video mode */
void
bochs_vbe_display(uint16_t width, uint16_t height, uint16_t depth);

/* put a pixel on screen */
void
(*bochs_vbe_putpixel)(uint16_t x, uint16_t y, uint32_t color);

/* putchar in graphic mode */
void
bochs_vbe_text_putchar(const char c);

void
bochs_vbe_text_puts(const char* str);

#endif
