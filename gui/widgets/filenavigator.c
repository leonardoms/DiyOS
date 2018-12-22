#include <gui.h>

void file_navigator_keydown(struct widget* widget, uint32_t key);
void file_navigator_mouse_event(struct widget* widget, int32_t x, int32_t y, uint32_t flags);
void file_navigator_focus(struct widget* widget);
void file_navigator_loose_focus(struct widget* widget);
void file_navigator_choose(struct widget* widget);

file_navigator_t*
file_navigator_create(uint8_t* directory, widget_t* parent) {
    file_navigator_t* fn;

    fn = (file_navigator_t*)malloc(sizeof(struct file_navigator));

    if( fn == NULL )
      return NULL;

    memset(fn, 0, sizeof(struct file_navigator));

    WIDGET(fn)->class = W_FILE_NAVIGATOR;
    WIDGET(fn)->x = 0;
    WIDGET(fn)->y = 0;
    WIDGET(fn)->w = (parent == NULL) ? 0 : parent->w - parent->padding_left - parent->padding_right;
    WIDGET(fn)->h = (parent == NULL) ? 0 : parent->h - parent->padding_top - parent->padding_bottom;;
    WIDGET(fn)->visible = W_VIS_PARENT;
    WIDGET(fn)->fgcolor = (color_t){ 0x606060 };
    WIDGET(fn)->bgcolor = (color_t){ 0xF0F0F0 };
    widget_set_padding(WIDGET(fn),0,0,0,0);
    WIDGET(fn)->OnPaint = NULL;
    WIDGET(fn)->OnKeyUp = NULL;
    WIDGET(fn)->OnKeyDown = file_navigator_keydown;
    WIDGET(fn)->OnMouseEvent = file_navigator_mouse_event;
    WIDGET(fn)->OnFocus = file_navigator_focus;
    WIDGET(fn)->OnLooseFocus = file_navigator_loose_focus;
    fn->OnChoose = file_navigator_choose;

    fn->directory = NULL;
    fn->files = NULL;

    fn->count = 0;
    fn->selected = -1;
    widget_set_parent(WIDGET(fn), parent);

    if( directory )
      file_navigator_set(fn, directory);

    return fn;
}

void
file_navigator_mouse_event(struct widget* widget, int32_t x, int32_t y, uint32_t flags) {
  if( flags & 1 )
    widget_set_focus(widget);
}

void
file_navigator_choose(struct widget* widget) {
  uint32_t i = 0;

  list_t *l = list_get(FILE_NAVIGATOR(widget)->files, FILE_NAVIGATOR(widget)->selected);

  if( l ) {
    if( ((struct dirent*)l->data)->flags & FS_DIRECTORY ) {
      // open dir
      if( !strcmp(((struct dirent*)l->data)->name,"..") ) {

        i = strlen(FILE_NAVIGATOR(widget)->directory);

        while( FILE_NAVIGATOR(widget)->directory[i] != '/' || i > 0)
          i--;
        FILE_NAVIGATOR(widget)->directory[i+1] = 0;

        file_navigator_set(FILE_NAVIGATOR(widget), strdup(FILE_NAVIGATOR(widget)->directory));

      } else {

        if( FILE_NAVIGATOR(widget)->directory[strlen(FILE_NAVIGATOR(widget)->directory)-1] != '/')
          file_navigator_set(FILE_NAVIGATOR(widget), strcat(strcat(FILE_NAVIGATOR(widget)->directory,"/"),((struct dirent*)l->data)->name));
        else
          file_navigator_set(FILE_NAVIGATOR(widget), strcat(FILE_NAVIGATOR(widget)->directory,((struct dirent*)l->data)->name) );

      }

    } else {
      // do open file
    }

  }
}

void
file_navigator_keydown(struct widget* widget, uint32_t key) {
    int32_t i = 0;

    switch (key) {
      case '\n':
        if( FILE_NAVIGATOR(widget)->selected >= 0 ) {
          FILE_NAVIGATOR(widget)->OnChoose(widget);
        }
        break;
      case KEY_DOWN:
      case KEY_RIGHT:
        FILE_NAVIGATOR(widget)->selected++;
        if(FILE_NAVIGATOR(widget)->selected >= FILE_NAVIGATOR(widget)->count)
          FILE_NAVIGATOR(widget)->selected--;
        break;
      case KEY_UP:
      case KEY_LEFT:
      FILE_NAVIGATOR(widget)->selected--;
      if(FILE_NAVIGATOR(widget)->selected < 0)
        FILE_NAVIGATOR(widget)->selected = 0;
        break;
      default:
        break;
    }
}

void
file_navigator_focus(struct widget* widget) {
    widget->bgcolor = (color_t){ 0xF8F8E0 };
}

void
file_navigator_loose_focus(struct widget* widget) {
    widget->bgcolor = (color_t){ 0xF0F0F0 };
}

void
file_navigator_set(file_navigator_t* fn, uint8_t* directory) {

  if( fn->directory ) {
    free(fn->directory);
    // list_destroy(fn->files);
    fn->files = NULL;
    fn->count = 0;
    fn->selected = -1;
  }

  if( directory ) {
    struct dirent* d = NULL;

    d = (struct dirent*)malloc(sizeof(struct dirent));
    strcpy(d->name,"..");
    d->flags = FS_DIRECTORY;
    fn->files = list_add(fn->files, d);
    fn->count++;

    while( (d = readdir(directory, d)) != NULL ) {
      fn->files = list_add(fn->files, d);
      fn->count++;
    }

    fn->directory = strdup(directory);

    if( fn->OnChoose_user )
      fn->OnChoose_user(WIDGET(fn));
  }

}

void
file_navigator_draw(file_navigator_t* fn) {
  uint32_t x0, y0, x = 0, y = 0, i = 0, f = 0;
  widget_t* parent = WIDGET(fn)->parent;

  if(!parent)
    return;

  widget_absolute_xy(WIDGET(fn), &x0, &y0);

  gfx_rect(x0,y0,x0+WIDGET(fn)->w,y0+WIDGET(fn)->h,WIDGET(fn)->bgcolor); // draw backgound area

  if(fn->files == NULL)
    return;

  x0 += WIDGET(fn)->padding_left;
  y0 += WIDGET(fn)->padding_top;

  list_t* l;
  while( (l = list_get(fn->files, f)) ) {
    uint8_t* text = ((struct dirent*)l->data)->name;

    if(((struct dirent*)l->data)->flags & FS_DIRECTORY )
      iconset_draw(6,1,x0,y0 + y);
    else
      iconset_draw(6,2,x0,y0 + y);

    x += 2; // 24px

    while(text[i]) {
        if(((x+1)*8 + WIDGET(fn)->padding_left + WIDGET(fn)->padding_right) > (parent->w - parent->padding_left - parent->padding_right) ) {// text wrap
          break;
        }


        if( y > (parent->h - parent->padding_top - parent->padding_bottom))
          return; // out of parent area!

        gfx_putchar(x0 + x*8 + 4, y0 + y + 6,
                    WIDGET(fn)->fgcolor,
                    (fn->selected == f) ? (color_t){0xFFDEAD} : WIDGET(fn)->bgcolor,
                    text[i]);
      i++;
      x++;
    }
    y += 16;
    x = 0;
    i = 0;
    f++;
  }
}
