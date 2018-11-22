#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <gui.h>

typedef struct button {
  struct widget widget;

  uint8_t       enabled;
} label_t;

button_t*
button_create(widget_t* parent);

#endif
