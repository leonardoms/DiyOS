
#include <gui.h>
#include <drivers/mouse.h>

void gui_draw();
void gui_desktop_create();
void gui_pointer_draw(uint32_t x, uint32_t y);
int32_t pointerX, pointerY;

#define POINTER_H 7
#define POINTER_W 5

widget_t *desktop_window, *desktop_taskbar;
task_t* gui_task;

#define TASK_BAR_HEIGHT 32

void
gui_main() {
  message_t* msg;
  struct mouse_packet* pkt;
  window_t*  window;

  while(!gfx_is_ready());

  printf("GUI started.\n");

  gui_desktop_create();

  task_listen( KEYBOARD | MOUSE ); // listen for events

  while(1) {
    disable();
    while( (msg = message()) != NULL ) { // read all messages
        switch(msg->from) {
            case KEYBOARD:
                // keyboard packet: byte1 = status; byte0 = key;
                // byte1: 0 = Press(1)/Release(0);
                //
                printf("GUI SERVER: key %s (0x%x).\n",
                              (((uint32_t)msg->data >> 8) & 1) ? "pressed" : "released",
                              (uint8_t)msg->data );

                window = gui_get_active_window();
                if( window == NULL )
                  break;

                if(window->focus) {
                  if((((uint32_t)msg->data >> 8) & 1)) {
                    if(window->focus->OnKeyDown)
                      window->focus->OnKeyDown(window->focus,((uint32_t)msg->data & 0xFF));
                  } else {
                    if(window->focus->OnKeyUp)
                      window->focus->OnKeyUp(window->focus,((uint32_t)msg->data & 0xFF));
                  }
                }

                break;
            case MOUSE:
                pkt = (struct mouse_packet*)msg->data;
                if( pkt ){
                  // printf("GUI SERVER: mouse flags=0x%x, dx=%d, dy=%d\n", pkt->flags, pkt->dx, pkt->dy);
                  pointerX += pkt->dx;
                  pointerY -= pkt->dy;

                  if(pointerX < 0)
                    pointerX = 0;
                  if(pointerX > gfx_width())
                    pointerX = gfx_width();
                  if(pointerY < 0)
                    pointerY = 0;
                  if(pointerY > gfx_height())
                    pointerY = gfx_height();

                  printf("GUI SERVER: mouse at (%d,%d)\n", pointerX, pointerY);
                }
                break;
            default:
                break;
          }
        message_destroy(msg);
      }

      gui_draw();
      enable();
      task_block();
    }

}

void
gui_desktop_create() {

    // the desktop area
    desktop_window = widget_create(0, 0, 0, gfx_width(), gfx_height() - TASK_BAR_HEIGHT - 1, NULL);
    desktop_window->bgcolor = (color_t){96,96,255};
    desktop_taskbar = widget_create(0, 0, gfx_height() - TASK_BAR_HEIGHT, gfx_width(), TASK_BAR_HEIGHT, NULL);
    desktop_taskbar->bgcolor = (color_t){224,224,224};
#if 1
    widget_t  *wnd, *wnd1, *lbl, *btn, *edt;
    wnd1 = WIDGET(window_create(400,300));
    lbl = WIDGET(label_create("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam venenatis viverra quam, id pretium mi maximus quis. Vestibulum vestibulum arcu in rutrum sagittis. Etiam interdum nec mi sit amet tincidunt.", wnd1));
    window_set_name(WINDOW(wnd1), "another window");
    // window_move(WINDOW(wnd1),10,10); // ERROR here!! (WTF)

    wnd = WIDGET(window_create(150,100));
    lbl = WIDGET(label_create("Welcome to the DiyOS basic GUI!", wnd));
    btn = button_create("OK", wnd);
    button_size( BUTTON(btn), (wnd->w - 40) / 2, (wnd->h - 12 - 10), 40, 12 );

    lbl = label_create("User:",wnd);
    edt = edit_create(wnd);
    WIDGET(lbl)->x = (wnd->w - 75) / 2 - 19;
    WIDGET(lbl)->y = (wnd->h - 60);
    WIDGET(edt)->x = (wnd->w - 75) / 2 + 20;
    WIDGET(edt)->y = (wnd->h - 60);
    WINDOW(wnd)->focus = edt;

    lbl = label_create("Passwd:",wnd);
    edt = edit_create(wnd);
    EDIT(edt)->text = "******";
    WIDGET(lbl)->x = (wnd->w - 75) / 2 - 35;
    WIDGET(lbl)->y = (wnd->h - 45);
    WIDGET(edt)->x = (wnd->w - 75) / 2 + 20;
    WIDGET(edt)->y = (wnd->h - 45);

    window_set_name(WINDOW(wnd), "Welcome Message!");
    gui_set_active_window(wnd);
    window_move(WINDOW(wnd),50,70);

#endif

    // the taskbar space
    widget_set_padding(desktop_window,0,0,0,/*TASK_BAR_HEIGHT*/0); // childs cannot use TaskBar area

    // taskbar_create_windows(desktop_taskbar);

    // mouse position
    pointerX = (gfx_width() - POINTER_W) / 2;
    pointerY = (gfx_height() - POINTER_H) / 2;
}

void
gui() {
    gui_task = task_create((uint32_t)gui_main, "gui", TS_READY );
    task_add(gui_task);
}

widget_t*
gui_widget_root() {
    return desktop_window;
}

void
gui_pointer_draw(uint32_t x, uint32_t y) {

  // gfx_rect( x - POINTER_W/2 - 1, y - POINTER_H/2 - 1,
  //           x + POINTER_W/2 + 1, y + POINTER_H/2 + 1,
  //           (color_t) {64,64,64} );
  //
  // gfx_rect( x - POINTER_W/2, y - POINTER_H/2,
  //           x + POINTER_W/2, y + POINTER_H/2,
  //           (color_t) {255,255,0} );
  gfx_rect( x , y ,
            x + 4, y + 4,
            (color_t) {255,0,0} );
}

void
taskbar_create_windows(widget_t* taskbar) {
  widget_t* desktop = gui_widget_root();
  widget_t* child;
  widget_t* lbl;
  uint32_t x = 0;

  if(!taskbar || !desktop )
    return;

  child = desktop->child;
  while( child ) {

    if(child->class == W_WINDOW) {
      lbl = WIDGET(label_create(WINDOW(child)->name, taskbar));
      lbl->x = x;
      x += lbl->w + lbl->padding_right;
    }

    child = child->next;
  }
}

void
gui_set_active_window(window_t* window) {
  widget_t* child, *root;

  root = gui_widget_root();
  if(root == NULL)
    return;

  child = root->child;
  while( child ) {
    if(child->class == W_WINDOW) {
      if(child == window)
        WINDOW(child)->active = 1;
      else
        WINDOW(child)->active = 0;
    }
    child = child->next;
  }
}

window_t*
gui_get_active_window() {
  //TODO: create GUI class struct with active_window member
  widget_t* child, *root;

  root = gui_widget_root();
  if(root == NULL)
    return;

  child = root->child;
  while( child ) {
    if(child->class == W_WINDOW) {
      if(WINDOW(child)->active == 1)
        return child;
    }
    child = child->next;
  }

  return NULL;
}

void
gui_draw_wallpaper() {
  // uint32_t i, j;
  //
  // for(j = 0; j < 480; j++)
  // for(i = 0; i < 640; i++)
  //   gfx_put_pixel(i,j,(color_t){96,96,255});
}

void
gui_draw() {
  gui_draw_wallpaper();
  widget_draw(desktop_window);
  widget_draw(desktop_taskbar);

  gui_pointer_draw((uint32_t)pointerX, (uint32_t)pointerY);
}
