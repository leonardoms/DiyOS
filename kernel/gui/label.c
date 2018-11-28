#include <gui.h>

void
label_size_request(label_t* lbl);

label_t*
label_create(const char* text, widget_t* parent) {
  label_t* lbl = (label_t*)malloc(sizeof(struct label));

  if( !lbl ) return NULL;

  memset(lbl, 0, sizeof(struct label));

  WIDGET(lbl)->class = W_LABEL;
  WIDGET(lbl)->x = 0;
  WIDGET(lbl)->y = 0;
  WIDGET(lbl)->w = 0;
  WIDGET(lbl)->h = 0;
  WIDGET(lbl)->visible = W_VIS_PARENT;
  WIDGET(lbl)->fgcolor = (color_t){ 96, 96, 96 };
  WIDGET(lbl)->bgcolor = (color_t){ 224, 224, 224 };
  widget_set_padding(WIDGET(lbl),2,0,0,0);
  WIDGET(lbl)->OnPaint = NULL;
  WIDGET(lbl)->OnKeyUp = NULL;
  WIDGET(lbl)->OnKeyDown = NULL;
  lbl->text = NULL;

  widget_set_parent(WIDGET(lbl), parent);

  label_set_text(lbl,text);
  label_set_linespace(lbl,2);

  return lbl;
}

// update size for label
void
label_size_request(label_t* lbl) {
  widget_t* parent;
  uint32_t txt_sz, txt_width, txt_height;

  if(lbl == NULL)
    return;

  if(lbl->text == NULL) {
    WIDGET(lbl)->w = 8;
    WIDGET(lbl)->h = 8;
    return;
  }

  txt_sz = strlen(lbl->text);
  txt_width = txt_sz * 8; // 8 = font width
  parent = WIDGET(lbl)->parent;

  if(parent) {
    if(txt_width > (parent->w - parent->padding_left - parent->padding_right)) {
      WIDGET(lbl)->w = parent->w - parent->padding_left - parent->padding_right;
      //calculate rows
      txt_height = txt_width / WIDGET(lbl)->w;
      if(txt_width % WIDGET(lbl)->w)
        txt_height++;

      // txt_height += WIDGET(lbl)->padding_top + WIDGET(lbl)->padding_bottom;
      // txt_height *= 8;
      WIDGET(lbl)->h = (txt_height + lbl->linespace) * 8 + WIDGET(lbl)->padding_top + WIDGET(lbl)->padding_bottom;
    }
  } else {
    WIDGET(lbl)->w = txt_width;
    WIDGET(lbl)->h = 8 + WIDGET(lbl)->padding_top + WIDGET(lbl)->padding_bottom;
  }
}

void
label_draw(label_t* lbl) {
  uint32_t x0, y0, x = 0, y = 0, i = 0;
  widget_t* parent = WIDGET(lbl)->parent;

  if(!parent)
    return;

  // label_size_request(lbl);

  widget_absolute_xy(WIDGET(lbl), &x0, &y0);

  gfx_rect(x0,y0,x0+WIDGET(lbl)->w,y0+WIDGET(lbl)->h,WIDGET(lbl)->bgcolor); // draw backgound area

  if(lbl->text == NULL)
    return;

  x0 += WIDGET(lbl)->padding_left;
  y0 += WIDGET(lbl)->padding_top;

  while(lbl->text[i]) {
    if(((x+1)*8 + WIDGET(lbl)->padding_left + WIDGET(lbl)->padding_right) > (parent->w - parent->padding_left - parent->padding_right) ) {// text wrap
      x = 0;
      y += 8 + lbl->linespace;
    }

    if( y > (parent->h - parent->padding_top - parent->padding_bottom))
      return; // out of parent area!

    gfx_putchar(x0 + x*8, y0 + y,
                WIDGET(lbl)->fgcolor,
                WIDGET(lbl)->bgcolor,
                lbl->text[i]);
    i++;
    x++;
  }
}

void
label_set_linespace(label_t* lbl, uint8_t space) {
    lbl->linespace = space;
}

void
label_set_text(label_t* lbl, const char* text) {

  if(lbl == NULL)
    return;

  if(lbl->text)
    free(lbl->text);

  if(text == NULL) {
    lbl->text = NULL;
    return;
  }

  lbl->text = (char*)malloc(strlen(text)+1);

  memcpy((void*)text, (void*)lbl->text, strlen(text) + 1);

  label_size_request(lbl);
}
