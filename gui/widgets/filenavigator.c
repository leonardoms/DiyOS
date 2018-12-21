#include <gui.h>

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
    WIDGET(fn)->bgcolor = (color_t){ 0xF8F8E0 };
    widget_set_padding(WIDGET(fn),0,0,0,0);
    WIDGET(fn)->OnPaint = NULL;
    WIDGET(fn)->OnKeyUp = NULL;
    WIDGET(fn)->OnKeyDown = NULL;

    fn->directory = NULL;
    fn->files = NULL;

    widget_set_parent(WIDGET(fn), parent);

    if( directory )
      file_navigator_set(fn, directory);

    return fn;
}

void
file_navigator_set(file_navigator_t* fn, uint8_t* directory) {

  if( fn->directory ) {
    free(fn->directory);
    // list_destroy(fn->files);
    fn->files = NULL;
  }

  if( directory ) {
    struct dirent* d = NULL;
    while( (d = readdir(directory, d)) != NULL ) {
      fn->files = list_add(fn->files, d);
    }
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
  while( (l = list_get(fn->files, f++)) ) {
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
                    WIDGET(fn)->bgcolor,
                    text[i]);
      i++;
      x++;
    }
    y += 16;
    x = 0;
    i = 0;
  }
}
