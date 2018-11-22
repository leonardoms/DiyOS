#include <gui.h>

uint32_t id = 0;

widget_t*
widget_create(uint32_t class, int32_t x, int32_t y, uint32_t w, uint32_t h,
      struct widget* parent) {

    widget_t* wgt = (widget_t*)kmalloc(sizeof(struct widget));
    widget_t* chld;

    if( !wgt ) return NULL;

    wgt->next = NULL;
    wgt->child = NULL;
    wgt->class = class;
    wgt->x = x;
    wgt->y = y;
    wgt->w = w;
    wgt->h = h;
    wgt->visible = W_VIS_PARENT;
    wgt->fgcolor = (color_t){ 0, 0, 0 };
    wgt->bgcolor = (color_t){ 255, 255, 255 };
    wgt->padding_top = 2; // 2px padding
    wgt->padding_left = 2; // 2px padding
    wgt->padding_right = 2; // 2px padding
    wgt->padding_bottom = 2; // 2px padding
    wgt->focus = NULL;
    wgt->has_focus = 0;

    widget_set_parent(wgt, parent);

    return wgt;
}



void
widget_draw(struct widget* wgt) {
  uint32_t x0, x1, y0, y1;
  struct widget* chld, *parent;

  if(!wgt)
    return;

  switch (wgt->class) {
    case W_WINDOW:
      window_draw((struct window*)wgt);
      break;
    case W_LABEL:
      label_draw((struct label*)wgt);
      break;
    default:
      parent = wgt->parent;

      if(parent) {

        x0 = wgt->x + parent->padding_left;
        x1 = x0 + wgt->w;
        y0 = wgt->y + parent->padding_top;
        y1 = y0 + wgt->h;

        if( x1 > (parent->w - parent->padding_right) ) { // fits on parent width ?
            x1 -= x1 - (parent->w - parent->padding_right);
        }

        if( y1 > (parent->h - parent->padding_bottom) ) { // fits on parent height ?
            y1 -= y1 - (parent->w - parent->padding_bottom);
        }

        gfx_rect(x0 + parent->x, y0 + parent->y, x1 + parent->x, y1 + parent->y, wgt->bgcolor.r, wgt->bgcolor.g, wgt->bgcolor.b);

      } else {
        gfx_rect(wgt->x, wgt->y, wgt->x + wgt->w, wgt->y + wgt->h, wgt->bgcolor.r, wgt->bgcolor.g, wgt->bgcolor.b);
      }
      break;
  }

  if(wgt->OnPaint) {
    if(!wgt->OnPaint(wgt))
      return; // stop drawing childs if OnPaint return FALSE
  }

  chld = wgt->child;
  while(chld) {
    widget_draw(chld);
    chld = chld->next;
  }

}

void
widget_set_parent(widget_t* widget, widget_t* parent) {
  widget_t* chld;

  if(!widget)
    return;

  widget->parent = parent;

  if(parent)
  if(parent->child) {

    chld = parent->child;
    while(chld->next)
      chld = chld->next;

    chld->next = widget;
  } else parent->child = widget;
}

void
widget_show(struct widget* wgt) {
    if(!wgt) return;

    switch (wgt->visible) {
      // case W_VIS_HIDDEN:
      //   break;
      // case W_VIS_PARENT:
      //   if(!wgt->parent) {
      //     // default for root widget is W_VIS_VISIBLE, draw it.
      //
      //   } else
      default:
        widget_draw(wgt);
        break;
    }
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
      widget->OnPaint = (Paint)ptr;
      break;
    case ON_KEYUP:
      widget->OnKeyUp = (KeyUp)ptr;
      break;
  }
}
