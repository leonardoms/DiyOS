
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
bmp_image_t* cursor_image = NULL;

#define TASK_BAR_HEIGHT 32

void
gui_main() {
  message_t* msg;
  struct mouse_packet* pkt;
  window_t*  window;

  debug_printf("GUI Task.\n");

  while(!gfx_is_ready());

  printf("GUI started.\n");

  disable();

  gui_desktop_create();
  gui_cursor_create();
  wallpaper();
  iconset("/ram/ui/ui-icons.bmp", 16, 16);

  task_listen( KEYBOARD | MOUSE ); // listen for events

  // wallpaper_draw_all();
  gui_draw();
  gfx_flip();

  while(1) {
    disable();
    while( (msg = message()) != NULL ) { // read all messages
        switch(msg->from) {
            case KEYBOARD:
                // keyboard packet: byte1 = status; byte0 = key;
                // byte1: 0 = Press(1)/Release(0);
                //
                // printf("GUI SERVER: key %s (0x%x).\n",
                //               (((uint32_t)msg->data >> 8) & 1) ? "pressed" : "released",
                //               (uint8_t)msg->data );

                window = gui_get_active_window();
                if( window == NULL )
                  break;

                // if(window->focus) {
                  if((((uint32_t)msg->data >> 8) & 1)) {
                    WIDGET(window)->OnKeyDown(WIDGET(window),((uint32_t)msg->data & 0xFF));
                    //
                    // if(window->focus->OnKeyDown)
                    //   window->focus->OnKeyDown(window->focus,((uint32_t)msg->data & 0xFF));
                  } else {
                    WIDGET(window)->OnKeyUp(WIDGET(window),((uint32_t)msg->data & 0xFF));
                    // if(window->focus->OnKeyUp)
                    //   window->focus->OnKeyUp(window->focus,((uint32_t)msg->data & 0xFF));
                  }
                // }

                break;
            case MOUSE:
                pkt = (struct mouse_packet*)msg->data;
                if( pkt ){
                  // debug_printf("GUI SERVER: mouse flags=0x%x, dx=%x, dy=%x\n", pkt->flags, pkt->dx, pkt->dy);
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

                  // if( pkt->flags & 1 )
                  //   window_move(gui_get_active_window(), pointerX, pointerY);
                  window = gui_get_active_window();
                  if( window == NULL )
                    break;

                  if( point_is_inside(WIDGET(window), pointerX, pointerY) ) {
                      if( pkt->dx || pkt->dy )
                        WIDGET(window)->OnMouseMove(WIDGET(window), pointerX - WIDGET(window)->x, pointerY - WIDGET(window)->y, pkt->flags);

                      WIDGET(window)->OnMouseEvent(WIDGET(window), pointerX - WIDGET(window)->x, pointerY - WIDGET(window)->y, pkt->flags);
                  } else {
                    // check if has clicked on other non-active window
                    widget_t* chld = desktop_window->child;
                    while(chld) {
                      if(chld != WIDGET(window) )
                      if( point_is_inside(chld, pointerX, pointerY) ) {
                          if(chld->OnMouseEvent)
                            chld->OnMouseEvent(chld, pointerX - chld->x, pointerY - chld->y, pkt->flags);
                          break;
                      }
                      chld = chld->next;
                    }
                  }

                  // printf("GUI SERVER: mouse at (%d,%d)\n", pointerX, pointerY);
                }
                break;
            default:
                break;
          }
        message_destroy(msg);
      }
      //TODO: every window has your own video memory buffer,
      // gui_draw function make a composite for modified area and send to video memory!
      // wallpaper_draw_area(0,0,100,150);
      // wallpaper_draw_all();
      gui_draw();
      iconset_draw(6,1,0,0);
      gfx_flip();
      enable();
      task_block();
    }

}

void
gui_cursor_create() {
  cursor_image = bmp_image_from_file("/ram/ui/cursor_normal.bmp");
}

void
cursor_draw() {
  if( cursor_image )
      gfx_draw_data_with_alfa(cursor_image->data, cursor_image->width, cursor_image->height, pointerX, pointerY);
}

void
gui_desktop_create() {

    // the desktop area
    desktop_window = widget_create(0, 0, 0, gfx_width(), gfx_height() - TASK_BAR_HEIGHT - 1, NULL);
    // desktop_window->bgcolor = (color_t){96,96,255};
    desktop_window->visible = W_VIS_HIDDEN;

    // the taskbar space
    widget_set_padding(desktop_window,0,0,0,TASK_BAR_HEIGHT); // childs cannot use TaskBar area

    desktop_taskbar = widget_create(0, 0, gfx_height() - TASK_BAR_HEIGHT, gfx_width(), gfx_height(), NULL);

    // mouse position
    pointerX = (gfx_width() - POINTER_W) / 2;
    pointerY = (gfx_height() - POINTER_H) / 2;
    if(pointerX < 0) pointerX = 0;
    if(pointerY < 0) pointerY = 0;

#if 1
    // widget_t  *wnd, *wnd1, *lbl, *btn, *edt, *fn;
    //
    // uint8_t buff[32];
    // uint32_t sz = 0;
    // buff[0] = 0;
    // int32_t fd = open("/ram/hello.txt", 1, 0);
    // if( fd >= 0) {
    //   sz = read(fd, buff, 32);
    //   buff[sz] = '\0';
    // }
    //
    //
    //
    // wnd1 = WIDGET(window_create(250,200));
    // lbl = WIDGET(label_create(buff, wnd1));
    // window_set_name(WINDOW(wnd1), "/ram/hello.txt");
    file_explorer();


#endif

}

void
gui() {
    gui_task = thread_create((uint32_t)gui_main, 1, 100, "gui");
    task_add(gui_task);
}

widget_t*
gui_widget_root() {
    return desktop_window;
}

void
gui_center_window(window_t* wnd) {
  window_move(wnd, (gfx_width() - WIDGET(wnd)->w) / 2, (gfx_height() - WIDGET(wnd)->h) / 2);
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
gui_draw() {
  widget_draw(desktop_window);
  widget_draw(WIDGET(gui_get_active_window()));
  widget_draw(desktop_taskbar);
  // gfx_put_pixel(pointerX, pointerY, (color_t){255,0,0});
  cursor_draw();
}
