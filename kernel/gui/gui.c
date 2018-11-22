
#include <gui.h>

widget_t *desktop_window, *desktop_taskbar;

#define TASK_BAR_HEIGHT 16

widget_t  *wnd, *lbl;

uint8_t
paint(struct widget* widget) {
  // create a red square widget as child, for testing...
  widget_t* w = widget_create(0, 0, 0, 20, 20, widget);
  w->bgcolor = (color_t) {255,0, 0};
  return 1;
}

void
gui_create() {
    desktop_window = widget_create(0, 0, 0, gfx_width(), gfx_height() - TASK_BAR_HEIGHT - 1, NULL);
    desktop_taskbar = widget_create(0, 0, gfx_height() - TASK_BAR_HEIGHT, gfx_width(), gfx_height(), NULL);
    desktop_taskbar->bgcolor = (color_t){32,32,32};

    widget_set_padding(desktop_window,0,0,0,TASK_BAR_HEIGHT); // childs cannot use TaskBar area
    desktop_window->bgcolor = (color_t){64,64,64};

    wnd = WIDGET(window_create(150,100));
    lbl = WIDGET(label_create("Testando um Label com texto grande em uma janela que esta contida no Desktop ...", wnd));

    // widget_set_callback(lbl,ON_PAINT,paint); // test...
}

widget_t*
gui_widget_root() {
    return desktop_window;
}

void
gui_draw() {
  widget_draw(desktop_taskbar);
  widget_draw(desktop_window);
  // widget_draw(wnd);
  // widget_draw(lbl);
}
