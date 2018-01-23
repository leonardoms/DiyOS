#include <small.h>

void
memcpy(void* src, void* dst, uint32_t size) {
  uint32_t i = 0;

  for(; i < size; i++)
    ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
}
