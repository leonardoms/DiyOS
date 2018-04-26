
#include <stdio.h>

int
main() {

  // puts("Hello World from User mode (ring3)!\n");
  while(1)
      putchar('U');

  return 0xDEADBEEF;
}
