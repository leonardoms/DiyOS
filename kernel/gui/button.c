#include <gui.h>

button_t*
button_create(const char* caption, widget_t* parent) {
  button_t* btn = (button_t*)malloc(sizeof(struct button));

  if( !btn ) return NULL;

  memset(btn, 0, sizeof(struct button));

  WIDGET(btn)->class = W_BUTTON;
  WIDGET(btn)->next = NULL;
  WIDGET(btn)->child = NULL;
  WIDGET(btn)->x = 0;
  WIDGET(btn)->y = 0;
  WIDGET(btn)->w = 30;
  WIDGET(btn)->h = 12;
  WIDGET(btn)->parent = parent;
  WIDGET(btn)->visible = W_VIS_PARENT;
  WIDGET(btn)->fgcolor = (color_t){ 0, 0, 0 };
  WIDGET(btn)->bgcolor = (color_t){ 196, 196, 196 };
  widget_set_padding(WIDGET(btn),0,0,0,0);
  WIDGET(btn)->OnPaint = NULL;
  WIDGET(btn)->OnKeyUp = NULL;
  WIDGET(btn)->OnKeyDown = NULL;

  widget_set_parent(WIDGET(btn), parent);

  btn->label = label_create(caption, WIDGET(btn));
  WIDGET(btn->label)->bgcolor = WIDGET(btn)->bgcolor;

  WIDGET(btn->label)->x = 12; //(uint32_t)(WIDGET(btn->label)->w / 2);

  return btn;
}

void
button_draw(button_t* btn) {

  uint32_t  x0, y0;

  if( btn == NULL )
    return;

  if(WIDGET(btn)->parent == NULL)
    return;

  widget_absolute_xy(WIDGET(btn), &x0, &y0);

  // shadow
  gfx_rect( x0 + 1,
            y0 + 1,
            x0 + WIDGET(btn)->w,
            y0 + WIDGET(btn)->h,
            (color_t){64,64,64});

  // button
  gfx_rect( x0,
            y0,
            x0 + WIDGET(btn)->w - 1,
            y0 + WIDGET(btn)->h - 1,
            WIDGET(btn)->bgcolor );
}

void
button_size(button_t* btn, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {

  if(btn == NULL)
    return;

  widget_move(WIDGET(btn), x, y);
  WIDGET(btn)->w = w;
  WIDGET(btn)->h = h;
  // widget_resize()

  label_size_request(btn->label);
}
