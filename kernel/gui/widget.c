#include <gui.h>

uint32_t id = 0;

widget_t*
widget_create(uint32_t class, int32_t x, int32_t y, uint32_t w, uint32_t h,
      struct widget* parent) {

    widget_t* widget = (widget_t*)malloc(sizeof(struct widget));
    widget_t* chld;

    if( !widget ) return NULL;

    widget->next = NULL;
    widget->child = NULL;
    widget->class = class;
    widget->x = x;
    widget->y = y;
    widget->w = w;
    widget->h = h;
    widget->visible = W_VIS_PARENT;
    widget->fgcolor = (color_t){ 0, 0, 0 };
    widget->bgcolor = (color_t){ 255, 255, 255 };
    widget->focus = NULL;
    widget->has_focus = 0;

    widget->OnPaint = NULL;
    widget->OnKeyUp = NULL;
    widget->OnKeyDown = NULL;

    widget_set_padding(widget, 2, 2, 2, 2);
    widget_set_parent(widget, parent);

    return widget;
}



void
widget_draw(struct widget* widget) {
  uint32_t x0, x1, y0, y1;
  struct widget* chld, *parent;

  if(!widget)
    return;

  // printf("draw 0x%x\n", widget);

  switch (widget->class) {
    case W_WINDOW:
      window_draw(WINDOW(widget));
      break;
    case W_LABEL:
      label_draw(LABEL(widget));
      break;
    case W_BUTTON:
      button_draw(BUTTON(widget));
      break;
    default:
      parent = widget->parent;
      if(parent) {
        x0 = widget->x + parent->padding_left;
        x1 = x0 + widget->w;
        y0 = widget->y + parent->padding_top;
        y1 = y0 + widget->h;

        if( x1 > (parent->w - parent->padding_right) ) { // fits on parent width ?
            x1 -= x1 - (parent->w - parent->padding_right);
        }

        if( y1 > (parent->h - parent->padding_bottom) ) { // fits on parent height ?
            y1 -= y1 - (parent->w - parent->padding_bottom);
        }

        gfx_rect(x0 + parent->x, y0 + parent->y, x1 + parent->x, y1 + parent->y, widget->bgcolor);

      } else {
        gfx_rect(widget->x, widget->y, widget->x + widget->w, widget->y + widget->h, widget->bgcolor);
      }
      break;
  }

  if(widget->OnPaint) {
    if(!widget->OnPaint(widget))
      return; // stop drawing childs if OnPaint return FALSE
  }

  chld = widget->child;
  while(chld) {
    widget_draw(chld);
    chld = chld->next;
  }

}

void
widget_set_parent(widget_t* widget, widget_t* parent) {
  widget_t* chld;

  if( widget == NULL )
    return;

  widget->parent = parent;

  if(parent) {

    if(parent->child) {

      chld = parent->child;
      while(chld->next)
        chld = chld->next;

      chld->next = widget;
    } else {
      parent->child = widget;
    }

    widget->next = NULL;

  }

}

void
widget_move(widget_t* widget, uint32_t x, uint32_t y) {
    widget_t* chld;

    if(!widget)
      return;

    widget->x = x;
    widget->y = y;

}

void
widget_set_focus(widget_t* widget) {
    widget_t* w, *tmp;

    //TODO: erase previous focus (find toplevel widget, roll widget->focus until has_focus, and ... make it 0)
    widget->has_focus = 1;
    tmp = widget;

    w = widget->parent;
    while(w) {
      w->focus = tmp;
      tmp = w;
      w = w->parent;
    }
}

void
widget_set_padding(widget_t* widget, uint32_t top, uint32_t left, uint32_t right, uint32_t bottom) {
    widget->padding_left = left;
    widget->padding_top = top;
    widget->padding_right = right;
    widget->padding_bottom = bottom;
}

void
widget_set_callback(widget_t* widget, uint8_t callback, void* ptr) {
  if(!widget)
    return;

  switch (callback) {
    case ON_PAINT:
      widget->OnPaint_User = (Paint)ptr;
      break;
    case ON_KEYUP:
      widget->OnKeyUp_User = (KeyUp)ptr;
      break;
  }
}

void
widget_absolute_xy(widget_t* widget, uint32_t* x, uint32_t* y) {
    widget_t  *parent;

    if(widget == NULL || x == NULL || y == NULL )
      return;

    *x = widget->x;
    *y = widget->y;

    parent = widget->parent;
    while(parent) {
      *x += parent->x + parent->padding_left;
      *y += parent->y + parent->padding_top;
      parent = parent->parent;
    }

}
