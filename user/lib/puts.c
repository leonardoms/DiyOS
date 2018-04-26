
#include "stdio.h"

void puts(const char* str) {
  while(*str) {
    putchar(*str);
    str++;
  }
}
