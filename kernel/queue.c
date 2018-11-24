#include <queue.h>

void
queue_init(queue_t* q, uint16_t size) {
  q->front 	= 0;
  q->rear 	= -1;
  q->count 	= 0;
	q->size 	= size;
	q->data = (uint32_t*)malloc(sizeof(uint32_t*)*size);
}

uint32_t*
queue_peek(queue_t* q) {

  if( q->count == 0 )
    return NULL;

  return q->data[q->front];
}

uint32_t
queue_size(queue_t* q) {
  return q->count;
}

void
queue_add(queue_t* q, uint32_t* ptr) {

  if( queue_size(q) < q->size ) {
      if( q->rear == (q->size - 1) ) {
          q->rear = -1;
      }

      q->data[++q->rear] = ptr;
      q->count++;
  }
}

uint32_t*
queue_remove(queue_t* q) {

  uint32_t* ptr;

  if( q->count == 0 )
    return NULL;

  ptr = q->data[q->front++];

  if( q->front == q->size )
      q->front = 0;

  q->count--;

  return ptr;
}

void
queue_foreach(queue_t* q, queue_callback_t c, void* data) {
  int32_t i, i_max;

  if( q->rear > q->front )
    i = q->front;
  else
    i = q->rear;

  i_max = i + queue_size(q);

  for( ; i < i_max; i++ ) {
    c(q->data[i], data);
  }
}
