#include <ring.h>
#include <kernel/panic.h>

#define RING_MAGIC  0xefb1195d

uint8_t
ring_init(ring_t* r, uint32_t size) {

  if(r == NULL) {
    printf("ring_init from NULL buffer pointer.\n");
    return FALSE;
  }

  if( size <= 1 ) {
    printf("ring_init size must be > 1.\n");
    return FALSE;
  }

  r->lenght = size;
  r->write = r->read = r->count = 0;
  r->magic = RING_MAGIC;

  r->buffer = (uint8_t*)malloc(size);

  // printf("ring_init %d\n", size);

  return TRUE;
}

uint8_t
ring_is_full(ring_t* r) {

  if(r == NULL) {
    printf("ring_is_full from NULL buffer pointer.\n");
    return TRUE;
  }

  ASSERT_PANIC( r->magic == RING_MAGIC );

  return (r->count >= r->lenght);
}

uint8_t
ring_is_empty(ring_t* r) {

  if(r == NULL) {
    printf("ring_is_empty from NULL buffer pointer.\n");
    return TRUE;
  }

  ASSERT_PANIC( r->magic == RING_MAGIC );

  return (r->count == 0);
}

uint32_t
ring_size(ring_t* r) {

  if(r == NULL) {
    printf("ring_size from NULL buffer pointer.\n");
    return 0;
  }

  ASSERT_PANIC( r->magic == RING_MAGIC );

  return r->lenght;
}

uint8_t
ring_push(ring_t* r, uint8_t data) {

  // printf("r(0x%x) <== 0x%x w(%d/%d)\n", (uint32_t)r, data, r->count, r->lenght);

  if(r == NULL) {
    printf("ring_push from NULL buffer pointer.\n");
    return FALSE;
  }

  ASSERT_PANIC( r->magic == RING_MAGIC );

  if( r->count >= r->lenght ) {
    printf("ring_push: buffer is full.\n");
    return FALSE;
  }

  r->buffer[r->write++] = data;
  r->count++;

  if( (r->write == r->lenght) && (r->read > 0) )
    r->write = 0;

  return TRUE;
}

uint8_t
ring_push32(ring_t* r, uint32_t data) {

  return TRUE;
}

uint8_t
ring_pop(ring_t* r, uint8_t* data) {

  if(r == NULL) {
    printf("ring_pop from NULL buffer pointer. (returning Zero)\n");
    return 0;
  }

  ASSERT_PANIC( r->magic == RING_MAGIC );

  if( r->count == 0 ) {
    printf("ring_pop: buffer is empty.\n");
    return FALSE;
  }

  if( data != NULL )
    *data = r->buffer[r->read++];
  r->count--;

  if( r->read >= r->lenght )
    r->read = 0;

  return TRUE;
}

uint8_t
ring_pop32(ring_t* r, uint32_t* data) {

  return TRUE;
}
