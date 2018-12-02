#ifndef _BIOS_H_
#define _BIOS_H_

#include <ctypes.h>

uint8_t
bios_read_byte(uint16_t segment, uint16_t offset);

uint16_t
bios_read_word(uint16_t segment, uint16_t offset);

#endif
