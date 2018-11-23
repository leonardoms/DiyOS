#ifndef _VGA_H_
#define _VGA_H_

#include <task.h>
#include <panic.h>
#include <small.h>


#define RGB_TO_332(R,G,B)	((R & 0xE0) | ((G  >> 3 ) & 0x1C) | (B >> 6))

#define	VGA_AC_INDEX		  0x3C0
#define	VGA_AC_WRITE		  0x3C0
#define	VGA_AC_READ       0x3C1
#define	VGA_MISC_WRITE		0x3C2
#define VGA_SEQ_INDEX		  0x3C4
#define VGA_SEQ_DATA		  0x3C5
#define	VGA_DAC_READ_INDEX	0x3C7
#define	VGA_DAC_WRITE_INDEX	0x3C8
#define	VGA_DAC_DATA		  0x3C9
#define	VGA_MISC_READ		  0x3CC
#define VGA_GC_INDEX 		  0x3CE
#define VGA_GC_DATA 	    0x3CF
/*			COLOR emulation		MONO emulation */
#define VGA_CRTC_INDEX		0x3D4		/* 0x3B4 */
#define VGA_CRTC_DATA		  0x3D5		/* 0x3B5 */
#define	VGA_INSTAT_READ		0x3DA

#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21
#define	VGA_NUM_REGS		  (1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + \
				VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)

void
vga_set_graphic();

uint32_t
vga_width();

uint32_t
vga_height();

void
vga_flip();

void
vga_clear(uint8_t c);

void vga_write_pixel(unsigned x, unsigned y, unsigned char c);

void
vga_rect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t color);

void
vga_putchar(uint32_t x, uint32_t y, uint8_t fgcolor, uint8_t bgcolor, const char c);

#endif
