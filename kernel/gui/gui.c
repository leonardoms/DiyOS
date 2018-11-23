
#include <gui.h>

widget_t *desktop_window, *desktop_taskbar;
task_t* gui_task;

#define TASK_BAR_HEIGHT 16

widget_t  *wnd, *lbl;

uint8_t
paint(struct widget* widget) {
  // create a red square widget as child, for testing...
  widget_t* w = WIDGET(label_create("teste!",widget));
  w->bgcolor = (color_t) {255,0, 0};

  return 1;
}

void
gui_main() {

  task_listen(KEYBOARD); // listen for Keyboard events
  struct message* msg;

  while(1) {
    while( (msg = get_message()) != NULL ) {
        switch(msg->from) {
            case KEYBOARD:
                // wnd = WIDGET(window_create(150,100));
                // lbl = WIDGET(label_create("Testando um Label com texto grande em uma janela que esta contida no Desktop ...", wnd));
                break;
            case MOUSE:
                break;
            default:
                break;
          }
      }

      task_block();
    }

}

void
gui_create() {

    // the desktop area
    desktop_window = widget_create(0, 0, 0, gfx_width(), gfx_height() - TASK_BAR_HEIGHT - 1, NULL);
    desktop_taskbar = widget_create(0, 0, gfx_height() - TASK_BAR_HEIGHT, gfx_width(), gfx_height(), NULL);
    desktop_taskbar->bgcolor = (color_t){32,32,32};

    // the taskbar
    widget_set_padding(desktop_window,0,0,0,TASK_BAR_HEIGHT); // childs cannot use TaskBar area
    desktop_window->bgcolor = (color_t){64,64,64};
    //
    // wnd = WIDGET(window_create(150,100));
    // lbl = WIDGET(label_create("Testando um Label com texto grande em uma janela que esta contida no Desktop ...", wnd));

    // widget_set_callback(lbl,ON_PAINT,paint); // test...

    gui_task = task_create((uint32_t)gui_main, "gui", TS_READY );
    task_add(gui_task);
}



widget_t*
gui_widget_root() {
    return desktop_window;
}

void
gui_draw() {
  widget_draw(desktop_taskbar);
  widget_draw(desktop_window);
}
