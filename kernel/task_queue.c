#include <task.h>

void
task_queue_init(task_queue_t* q) {
  q->front = 0;
  q->rear = -1;
  q->count = 0;
}

task_t*
task_queue_peek(task_queue_t* q) {

  if( q->count == 0 )
    return NULL;

  return q->task[q->front];
}

uint32_t
task_queue_size(task_queue_t* q) {
  return q->count;
}

void
task_queue_insert(task_queue_t* q, task_t* t) {

  if( task_queue_size(q) < TASK_MAX ) {
      if( q->rear == (TASK_MAX - 1) ) {
          q->rear = -1;
      }

      q->task[++q->rear] = t;
      q->count++;
  }
}

task_t*
task_queue_remove(task_queue_t* q) {

  task_t* t;

  // if( q->count == 0 )
  //   return NULL;

  t = q->task[q->front++];

  if( q->front == TASK_MAX )
      q->front = 0;

  q->count--;

  return t;
}

void
task_queue_foreach(task_queue_t* q, task_queue_callback_t c, uint32_t* data) {
  int32_t i, i_max;

  if( q->rear > q->front )
    i = q->front;
  else
    i = q->rear;

  i_max = i + task_queue_size(q);

  for( ; i < i_max; i++ ) {
    c(q->task[i], data);
  }
}
