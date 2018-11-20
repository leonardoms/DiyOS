#ifndef _QUEUE_H
#define _QUEUE_H

#include <ctypes.h>

typedef struct {
	unsigned char *data;

	uint16_t size;
	uint16_t id_last;
	uint16_t id_first;
} queue_t;

uint8_t queue_get_data(queue_t *q, uint8_t *data);
uint8_t queue_insert_data(queue_t *q, uint8_t data);
uint8_t queue_init(queue_t *q, uint32_t sz);
// #define queue_destroy(q) free((q)->data)
#define queue_destroy(q)

#endif
