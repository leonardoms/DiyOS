
#include <gui.h>

void window_mouse_move(struct widget* widget, int32_t x, int32_t y, uint32_t flags);
void window_mouse_event(struct widget* widget, int32_t x, int32_t y, uint32_t flags);
void window_key_down(struct widget* widget, uint32_t key);
void window_key_up(struct widget* widget, uint32_t key);

window_t*
window_create(uint32_t w, uint32_t h) {
  window_t* wnd = (window_t*)malloc(sizeof(struct window));

  if( !wnd ) return NULL;

  memset(wnd, 0, sizeof(struct window));

  WIDGET(wnd)->class = W_WINDOW;
  WIDGET(wnd)->x = (gfx_width() - w) / 2;
  WIDGET(wnd)->y = 10;
  WIDGET(wnd)->w = w;
  WIDGET(wnd)->h = h;
  WIDGET(wnd)->visible = W_VIS_PARENT;
  WIDGET(wnd)->fgcolor = (color_t){ 0, 0, 0 };
  WIDGET(wnd)->bgcolor = (color_t){ 224, 224, 224 };
  widget_set_padding(WIDGET(wnd),2,2,2,2);
  WIDGET(wnd)->OnPaint = NULL;
  WIDGET(wnd)->OnKeyUp = window_key_up;
  WIDGET(wnd)->OnKeyDown = window_key_down;
  WIDGET(wnd)->OnMouseMove = window_mouse_move;
  WIDGET(wnd)->OnMouseEvent = window_mouse_event;

  wnd->name = NULL;
  wnd->active = 0;

  wnd->type = WINDOW_TOPLEVEL;

  // toplevel has decoration, update x,y
  WIDGET(wnd)->x += WINDOW_DECORATION_BORDER;
  WIDGET(wnd)->y += WINDOW_DECORATION_BAR;

  // wnd->name = "DiyOS";

  window_set_name(wnd,"DiyOS");

  widget_set_parent(wnd,gui_widget_root()); // the parent is the desktop area


  return wnd;
}

void
window_mouse_event(struct widget* widget, int32_t x, int32_t y, uint32_t flags) {
  widget_t* chld;

  if( flags & 1)
    gui_set_active_window(WINDOW(widget));

  chld = widget->child;
  while(chld) {
    if( point_is_inside(chld, x, y) ) {
        if(chld->OnMouseEvent)
          chld->OnMouseEvent(chld, x - chld->x, y - chld->y, flags);
        break;
    }
    chld = chld->next;
  }
}

void
window_mouse_move(struct widget* widget, int32_t x, int32_t y, uint32_t flags) {
  widget_t* chld;

  chld = widget->child;
  while(chld) {
    if( point_is_inside(chld, x, y) ) {
        if(chld->OnMouseMove)
          chld->OnMouseMove(chld, x - chld->x, y - chld->y, flags);
        break;
    }
    chld = chld->next;
  }
}

void
window_key_down(struct widget* widget, uint32_t key) {
  if( WINDOW(widget)->focus ) { // propagate to focused child
    if(WINDOW(widget)->focus->OnKeyDown)
      WINDOW(widget)->focus->OnKeyDown(WINDOW(widget)->focus, key);
  }
}

void
window_key_up(struct widget* widget, uint32_t key) {
  if( WINDOW(widget)->focus ) { // propagate to focused child
    if(WINDOW(widget)->focus->OnKeyUp)
      WINDOW(widget)->focus->OnKeyUp(WINDOW(widget)->focus, key);
  }
}

void
window_set_name(window_t* window, char* name) {

  if(window == NULL)
      return;

  // if(window->name)
  //   free(window->name);

  window->name = (uint8_t*)malloc(strlen(name) + 1);
  if( window->name != NULL)
    memcpy((void*)name, (void*)window->name, strlen(name) + 1);
}

char*
window_get_name(window_t* window) {
  return window->name;
}

void
window_set_type(window_t* window, window_type_t type) {
  window->type = type;
}

window_type_t
window_get_type(window_t* window) {
  return window->type;
}

void
window_move(window_t* window, uint32_t x, uint32_t y) {
  widget_move(WIDGET(window),x,y);
}

void
window_draw(window_t* window) {
  uint32_t x0, y0, i = 0;
  color_t border, tbar, title;

  if(window == NULL)
    return;

  border = (window->active == 1) ? (color_t){64,64,64} : (color_t){32,32,32};
  tbar = (window->active == 1) ? (color_t){32,32,96} : (color_t){64,64,64};

  title = (window->active == 1) ? (color_t){196,196,196} : (color_t){128,128,128};

  widget_absolute_xy(WIDGET(window), &x0, &y0);

  // shadow
  gfx_rect( x0 - WINDOW_DECORATION_BORDER + WINDOW_SHADOW_SIZE,
            y0 - WINDOW_DECORATION_BAR + WINDOW_SHADOW_SIZE,
            x0 + WIDGET(window)->w + WINDOW_DECORATION_BORDER + WINDOW_SHADOW_SIZE,
            y0 + WIDGET(window)->h + WINDOW_DECORATION_BORDER + WINDOW_SHADOW_SIZE,
            (color_t){96,96,96} );

  // title bar + borders
  gfx_rect( x0 - WINDOW_DECORATION_BORDER,
            y0 - WINDOW_DECORATION_BAR,
            x0 + WIDGET(window)->w + WINDOW_DECORATION_BORDER,
            y0 + WIDGET(window)->h + WINDOW_DECORATION_BORDER,
            border );

  // title bar internal
  gfx_rect( x0,
            y0 + WINDOW_DECORATION_BORDER - WINDOW_DECORATION_BAR,
            x0 + WIDGET(window)->w,
            y0 - WINDOW_DECORATION_BORDER * 2,
            tbar );

  // draw widget area
  gfx_rect( x0,
            y0,
            x0 + WIDGET(window)->w,
            y0 + WIDGET(window)->h,
            WIDGET(window)->bgcolor );

  // draw title
  if(window->name) {
    x0 += 1;
    y0 += WINDOW_DECORATION_BORDER + 1 - WINDOW_DECORATION_BAR;

    while(window->name[i]) {
      gfx_putchar(x0+i*8, y0,
                  title, tbar,
                  window->name[i]);
      i++;
    }
  }
}
