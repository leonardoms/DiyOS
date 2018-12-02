#include <stdio.h>

void memset(void* src, uint8_t value, uint32_t size) {
  uint32_t i = 0;

  for(;i < size; i++)
    ((uint8_t*)src)[i] = value;
}
