#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <gui.h>

typedef struct button {
  struct widget widget;

  label_t*  label;

  uint8_t     enabled;
  uint8_t     pressed;
} button_t;

#define BUTTON(b) ((button_t*)b)

button_t*
button_create(const char* caption, widget_t* parent);

void
button_size(button_t* btn, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

#endif
