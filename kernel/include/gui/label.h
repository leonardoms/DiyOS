#ifndef _LABEL_H_
#define _LABEL_H_

#include <gui.h>

typedef struct label {
  struct widget widget;

  uint8_t       *text;
} label_t;

label_t*
label_create(const char* text, widget_t* parent);

void
label_set_text(label_t* lbl, const char* text);
//
// char*
// label_get_text(label_t* window);

#endif
