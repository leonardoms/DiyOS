#include <gui.h>

void edit_keydown(struct widget* widget, uint32_t key);

edit_t*
edit_create(widget_t* parent) {
  edit_t* edt = (edit_t*)malloc(sizeof(struct edit));

  if( !edt ) return NULL;

  memset(edt, 0, sizeof(struct edit));

  WIDGET(edt)->class = W_EDIT;
  WIDGET(edt)->next = NULL;
  WIDGET(edt)->child = NULL;
  WIDGET(edt)->x = 0;
  WIDGET(edt)->y = 0;
  WIDGET(edt)->w = 75;
  WIDGET(edt)->h = 12;
  WIDGET(edt)->parent = parent;
  WIDGET(edt)->visible = W_VIS_PARENT;
  WIDGET(edt)->fgcolor = (color_t){ 64, 64, 64 };
  WIDGET(edt)->bgcolor = (color_t){ 255, 255, 255 };
  widget_set_padding(WIDGET(edt),0,0,0,0);
  WIDGET(edt)->OnPaint = NULL;
  WIDGET(edt)->OnKeyUp = NULL;
  WIDGET(edt)->OnKeyDown = edit_keydown;
  edt->length = 32; // default number of max chars

  edt->text = (uint8_t*)malloc(edt->length + 1);

  edt->cursor_position = 0;

  widget_set_parent(WIDGET(edt), parent);

  return edt;
}

void edit_keydown(struct widget* widget, uint32_t key) {

  if(EDIT(widget)->cursor_position >= EDIT(widget)->length)
    return;

  EDIT(widget)->text[EDIT(widget)->cursor_position++] = (uint8_t)key;

  // widget_draw(widget); // redraw the Edit

  if(widget->OnKeyDown_User)
    widget->OnKeyDown_User(widget,key);
}

void
edit_draw(edit_t* edt) {

  uint32_t  x0, y0;

  if( edt == NULL )
    return;

  if(WIDGET(edt)->parent == NULL)
    return;

  widget_absolute_xy(WIDGET(edt), &x0, &y0);

  // shadow
  // gfx_rect( x0 + 1,
  //           y0 + 1,
  //           x0 + WIDGET(edt)->w,
  //           y0 + WIDGET(edt)->h,
  //           (color_t){64,64,64});

  // border
  gfx_rect( x0,
            y0,
            x0 + WIDGET(edt)->w,
            y0 + WIDGET(edt)->h,
            (color_t){64,64,64});

  // edit
  gfx_rect( x0 + 1,
            y0 + 1,
            x0 + WIDGET(edt)->w - 1,
            y0 + WIDGET(edt)->h - 1,
            WIDGET(edt)->bgcolor );

  // draw text
  uint32_t i = 0;
  uint32_t x = 0;
  while( edt->text[i] ) {
    if( (x + 8) > WIDGET(edt)->w )
      break;
    gfx_putchar(x0 + x + 2, y0 + 2, WIDGET(edt)->fgcolor, WIDGET(edt)->bgcolor, edt->text[i]);
    x += 8;
    i++;
  }
}
