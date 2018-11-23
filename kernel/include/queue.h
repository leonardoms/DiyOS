#ifndef _QUEUE_H
#define _QUEUE_H

#include <ctypes.h>
#include <x86/memory.h>

typedef struct queue {
	uint32_t *data;

	uint16_t size, count;
	uint16_t rear;
	uint16_t front;
} queue_t;

typedef void (*queue_callback_t)(uint32_t* item, void* data); // item: the pointer stored on queue; data: a user defined data.

void
queue_init(queue_t* q, uint16_t size);

uint32_t
queue_size(queue_t* q); // number of elements

uint32_t*
queue_peek(queue_t* q); // get queue top, but dont remove from queue!

void
queue_add(queue_t* q, uint32_t* ptr); // add to queue

uint32_t*
queue_remove(queue_t* q) ; // get queue top and remove from list

void
queue_foreach(queue_t* q, queue_callback_t c, void* data);

#endif
