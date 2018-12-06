#ifndef _LIST_H_
#define _LIST_H_

#include <ctypes.h>
#include <stdlib.h>
#include <stdio.h>

#define LIST_MAGIC  0xC0FFBABE

typedef struct list {

  uint32_t  magic;
  void*     data;

  struct list* next;
} list_t;

typedef uint8_t (*list_iterator_t)(list_t* l, void* udata);

list_t*
list_add(list_t* l, void* data);

list_t*
list_foreach(list_t* l, list_iterator_t it, void* udata);

#endif
