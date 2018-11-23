
#include <gui.h>

window_t*
window_create(uint32_t w, uint32_t h) {
  window_t* wnd = (window_t*)kmalloc(sizeof(struct window));

  if( !wnd ) return NULL;

  memset(wnd, 0, sizeof(struct window));

  WIDGET(wnd)->class = W_WINDOW;
  WIDGET(wnd)->x = 5;
  WIDGET(wnd)->y = 5;
  WIDGET(wnd)->w = w;
  WIDGET(wnd)->h = h;
  WIDGET(wnd)->visible = W_VIS_PARENT;
  WIDGET(wnd)->fgcolor = (color_t){ 64, 64, 64 };
  WIDGET(wnd)->bgcolor = (color_t){ 255, 255, 255 };
  widget_set_padding(WIDGET(wnd),2,2,2,2);
  WIDGET(wnd)->OnPaint = NULL;
  WIDGET(wnd)->OnKeyUp = NULL;
  WIDGET(wnd)->OnKeyDown = NULL;

  wnd->type = WINDOW_TOPLEVEL;

  // toplevel has decoration, update x,y
  WIDGET(wnd)->x += WINDOW_DECORATION_BORDER;
  WIDGET(wnd)->y += WINDOW_DECORATION_BAR;

  wnd->name = "DiyOS";

  // window_set_name(wnd,"DiyOS");

  widget_set_parent(wnd,gui_widget_root()); // the parent is the desktop area


  return wnd;
}

void
window_set_name(window_t* window, char* name) {
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
window_draw(window_t* window) {
  uint32_t x0, y0, i = 0;
  color_t border, tbar;

  border = (color_t){WIDGET(window)->bgcolor.r >> 1 , WIDGET(window)->bgcolor.g >> 1, WIDGET(window)->bgcolor.b >> 1};
  tbar = (color_t){WIDGET(window)->bgcolor.r >> 2 , WIDGET(window)->bgcolor.g >> 2, WIDGET(window)->bgcolor.b >> 2};

  // title bar + borders
  gfx_rect( WIDGET(window)->x - WINDOW_DECORATION_BORDER,
            WIDGET(window)->y - WINDOW_DECORATION_BAR,
            WIDGET(window)->x + WIDGET(window)->w + WINDOW_DECORATION_BORDER,
            WIDGET(window)->y + WIDGET(window)->h + WINDOW_DECORATION_BORDER,
            border );

  // title bar internal
  gfx_rect( WIDGET(window)->x,
            WIDGET(window)->y + WINDOW_DECORATION_BORDER - WINDOW_DECORATION_BAR,
            WIDGET(window)->x + WIDGET(window)->w,
            WIDGET(window)->y - WINDOW_DECORATION_BORDER * 2,
            tbar );

  // draw widget area
  gfx_rect( WIDGET(window)->x,
            WIDGET(window)->y,
            WIDGET(window)->x + WIDGET(window)->w,
            WIDGET(window)->y + WIDGET(window)->h,
            WIDGET(window)->bgcolor );

  // draw title
  x0 = WIDGET(window)->x + 1;
  y0 = WIDGET(window)->y - WINDOW_DECORATION_BAR + WINDOW_DECORATION_BORDER + 1;

  while(window->name[i]) {
    gfx_putchar(x0+i*8, y0,
                WIDGET(window)->fgcolor, tbar,
                window->name[i]);
    i++;
  }

  // draw decoration
  // //bottom line
  // gfx_line( WIDGET(window)->x,
  //           WIDGET(window)->y + WINDOW_DECORATION_BAR + WIDGET(window)->h + WINDOW_DECORATION_BORDER ,
  //           WIDGET(window)->x + WIDGET(window)->w + WINDOW_DECORATION_BORDER * 2,
  //           WIDGET(window)->y + WINDOW_DECORATION_BAR + WIDGET(window)->h + WINDOW_DECORATION_BORDER,
  //           WIDGET(window)->bgcolor.r >> 1 , WIDGET(window)->bgcolor.g >> 1, WIDGET(window)->bgcolor.b >> 1);
  // // left line
  // gfx_line( WIDGET(window)->x,
  //           WIDGET(window)->y,
  //           WIDGET(window)->x,
  //           WIDGET(window)->y + WINDOW_DECORATION_BAR + WIDGET(window)->h + WINDOW_DECORATION_BORDER,
  //           WIDGET(window)->bgcolor.r >> 1 , WIDGET(window)->bgcolor.g >> 1, WIDGET(window)->bgcolor.b >> 1);
  // // right line
  // gfx_line( WIDGET(window)->x + WIDGET(window)->w + WINDOW_DECORATION_BORDER * 2,
  //           WIDGET(window)->y,
  //           WIDGET(window)->x + WIDGET(window)->w + WINDOW_DECORATION_BORDER * 2,
  //           WIDGET(window)->y + WINDOW_DECORATION_BAR + WIDGET(window)->h + WINDOW_DECORATION_BORDER,
  //           WIDGET(window)->bgcolor.r >> 1 , WIDGET(window)->bgcolor.g >> 1, WIDGET(window)->bgcolor.b >> 1);
  //
  // // title bar
  // gfx_rect( WIDGET(window)->x,
  //           WIDGET(window)->y,
  //           WIDGET(window)->x + WINDOW_DECORATION_BORDER * 2 + WIDGET(window)->w,
  //           WIDGET(window)->y + WINDOW_DECORATION_BAR,
  //           WIDGET(window)->bgcolor.r >> 1 , WIDGET(window)->bgcolor.g >> 1, WIDGET(window)->bgcolor.b >> 1 );

}
