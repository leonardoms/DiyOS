#ifndef _RING_H_
#define _RING_H_

#include <stdlib.h>

typedef struct ring {
  uint8_t*  buffer;
  uint32_t  head, tail;
  uint32_t  lenght;
} ring_t;

#endif
