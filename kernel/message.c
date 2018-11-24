
#include <message.h>

void
message_to(uint32_t to, void* data, uint32_t size) {
  task_t* t;
  message_t* msg;

  t = task_from_pid(to); // id of target task

  if(t == NULL) {
    printf("message_to(): invalid PID (%d)\n", to);
    return;
  }

  msg = (message_t*)malloc(sizeof(struct message));

  msg->from = task_pid();
  msg->to = to;
  msg->data = data;
  msg->size = size;

  queue_add(&t->message_queue, msg);

  task_wake(t);
}

message_t*
message() {
    task_t* t;

    t = task_get(); // get current task

    if(t == NULL) {
      printf("message(): invalid task.\n");
      return NULL;
    }

    return (message_t*)queue_remove(&t->message_queue);
}

void
message_destroy(message_t* msg) {
    free(msg);
}
