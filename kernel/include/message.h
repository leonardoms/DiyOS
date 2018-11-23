#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <task.h>

typedef struct message {
  uint32_t from, to;
  void* data;
  uint32_t size;
} message_t;

void
message_to(uint32_t to, void* data, uint32_t size); // send a message

message_t* message(); // read a message

void
message_destroy(message_t* msg); // must use after read the message

#endif
