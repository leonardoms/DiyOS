#ifndef _RING_H_
#define _RING_H_

#include <stdlib.h>

typedef struct ring {
  uint32_t  magic;
  uint8_t*  buffer;
  uint32_t  read, write;
  uint32_t  lenght, count;
} ring_t;

uint8_t
ring_init(ring_t* r, uint32_t size);

uint8_t
ring_is_full(ring_t* r);

uint8_t
ring_is_empty(ring_t* r);

uint32_t
ring_size(ring_t* r);

uint8_t
ring_push(ring_t* r, uint8_t data);

uint8_t
ring_pop(ring_t* r, uint8_t* data);

#endif
