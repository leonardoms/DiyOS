#ifndef _EDIT_H_
#define _EDIT_H_

#include <gui.h>

typedef struct edit {
  struct widget widget;

  uint8_t* text;
  uint8_t length;
  uint8_t cursor_position;
} edit_t;

#define EDIT(b) ((edit_t*)b)

edit_t*
edit_create(widget_t* parent);

#endif
