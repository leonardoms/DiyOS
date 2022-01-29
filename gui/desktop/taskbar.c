#include <gui.h>

widget_t *desktop_taskbar;

uint32_t taskbarH;

uint32_t
taskbar_height() {
  return taskbarH;
}

void create_taskbar() {
  widget_t* btn_app;

  taskbarH = 16;

  desktop_taskbar = widget_create(0, 0, gfx_height() - taskbarH, gfx_width(), gfx_height(), NULL);
  desktop_taskbar->bgcolor = (color_t){ 0xE4E4E4 };

  btn_app = WIDGET(button_create("Menu", desktop_taskbar));

  // btn_app->y = ( taskbarH - btn_app->h /* - padding top and bottom */ ) / 2;

  btn_app->w = 50;

}

void
taskbar_draw() {
  widget_draw(desktop_taskbar);
}
