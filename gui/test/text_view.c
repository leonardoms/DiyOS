#include <gui.h>

uint8_t buff[256];

void
text_view(uint8_t* file) {
  widget_t  *wnd1, *lbl;

  // debug_printf("text_view: %s\n", file);

  uint32_t sz = 0;
  buff[0] = 0;
  int32_t fd = open(file, 1, 0);
  if( fd >= 0) {
    sz = read(fd, buff, 256);
    buff[sz] = '\0';
  }

  wnd1 = WIDGET(window_create(250,200));
  lbl = WIDGET(label_create(buff, wnd1));
  window_set_name(WINDOW(wnd1), file);

  widget_t* awnd = WIDGET(gui_get_active_window());

  window_move(wnd1, awnd->x + 60, awnd->y + 60);

  gui_set_active_window(wnd1);
}
