#ifndef _WIDGET_
#define _WIDGET_

#define WIDGET(w) ((struct widget*)w)

enum { W_WIDGET = 0, W_WINDOW, W_LABEL, W_BUTTON };

enum { ON_PAINT = 1, ON_KEYUP };

typedef enum { W_VIS_HIDDEN, W_VIS_VISIBLE, W_VIS_PARENT } widget_visibility_t;

typedef struct widget {
  uint32_t  magic;

  struct widget* parent;
  struct widget* child;

  uint32_t  id;
  uint32_t  class;

  int32_t   x, y; // position relative to parent; -1 represent FILL_PARENT
  int32_t   w, h;
  uint32_t  padding_top, padding_left, padding_right, padding_bottom;

  color_t   fgcolor, bgcolor;

  widget_visibility_t visible;

  // Callbacks
  uint8_t (*OnPaint)(struct widget* widget);
  void (*OnKeyDown)(struct widget* widget, uint32_t key);
  void (*OnKeyUp)(struct widget* widget, uint32_t key);

  struct widget* focus;
  uint8_t   has_focus;

  struct widget* next;
} widget_t;

typedef uint8_t (*Paint)(struct widget* widget);  // if return FALSE stops drawing
typedef void (*KeyDown)(struct widget* widget, uint32_t key);
typedef void (*KeyUp)(struct widget* widget, uint32_t key);

widget_t*
widget_create(uint32_t class, int32_t x, int32_t y, uint32_t w, uint32_t h,
      struct widget* parent);

void
widget_show(struct widget* wgt);

void
widget_set_padding(widget_t* widget, uint32_t top, uint32_t left, uint32_t right, uint32_t bottom);

void
widget_set_parent(widget_t* widget, widget_t* parent);

void
widget_set_callback(widget_t* widget, uint8_t callback, void* ptr);

void
widget_move(widget_t* widget, uint32_t x, uint32_t y);

void
widget_absolute_xy(widget_t* widget, uint32_t* x, uint32_t* y);

#endif
