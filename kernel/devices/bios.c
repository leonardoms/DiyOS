// read legacy BIOS data
#include <kernel.h>

uint8_t
bios_read_byte(uint16_t segment, uint16_t offset) {
  uint32_t laddress = segment * 16 + offset;

  if((laddress < 0x400) || (laddress > 0xFFFFE)) {
    printf("bios_read_byte: invalid address %x:%x\n", segment, offset);
    return 0;
  }

  return ((uint8_t*)laddress)[0];
}

uint16_t
bios_read_word(uint16_t segment, uint16_t offset) {
  uint32_t laddress = segment * 16 + offset;

  if((laddress < 0x400) || (laddress > 0xFFFFE)) {
    printf("bios_read_word: invalid address %x:%x\n", segment, offset);
    return 0;
  }

  return ((uint16_t*)laddress)[0];
}
