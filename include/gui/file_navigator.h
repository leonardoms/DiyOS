#ifndef _FILE_NAVIGATOR_H_
#define _FILE_NAVIGATOR_H_

#include <gui.h>
#include <list.h>

typedef struct file_navigator {
  struct widget widget;

  struct {
    uint32_t x, y, w, h; // file entry relative position on widget
  } geometry[64];

  uint8_t       *directory;
  list_t        *files;
  uint32_t      count;
  int32_t       selected;
} file_navigator_t;

#define FILE_NAVIGATOR(f) ((file_navigator_t*)f)


file_navigator_t*
file_navigator_create(uint8_t* directory, widget_t* parent);

void
file_navigator_set(file_navigator_t* fn, uint8_t* directory);

#endif
