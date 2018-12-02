#include <stdio.h>

uint32_t
strlen(uint8_t* str) {
  uint32_t count = 0;

  while(str[count])
    count++;

  return count;
}
