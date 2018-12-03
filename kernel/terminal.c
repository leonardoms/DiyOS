#include <kernel.h>

static uint8_t terminal_id = 0;

static terminal_t  *term0 = NULL;

terminal_t*
terminal(void) {
  terminal_t* term;

  term = (terminal_t*)malloc(sizeof(struct terminal));

  if(term == NULL)
    return NULL;

  term->cols = 25;
  term->rows = 80;
  term->color = FC_LGRAY | BC_BLACK;
  term->buffer = (uint8_t*)malloc((uint32_t)(term->cols * term->rows * 2 * 4)); // can roll 4x the rows size

  if(term->buffer == NULL) {
    free(term);
    return NULL;
  }

  term->id = terminal_id++;

  if( term0 == NULL )
    term0 = term;

  return term;
}

void
terminal_goto(terminal_t* term, uint8_t _row, uint8_t _col) {
  if( term->col < term->cols )
    term->col = _col;
  else
    term->col = term->cols - 1;

  if( term->row < term->rows )
    term->row = _row;
  else
    term->row = term->rows - 1;

  terminal_cursor_update();
}

void
terminal_color(terminal_t* term, uint8_t _color) {
  term->color = _color;
}

void
terminal_getpos(terminal_t* term, uint8_t* _row, uint8_t* _col) {
  if(_row != NULL)
    *_row = term->row;
  if(_col != NULL)
    *_col = term->col;
}
void
terminal_get_color(terminal_t* term, uint8_t* _color) {
  if(_color != NULL)
    *_color = term->color;
}
//
// void terminal_newline() {
//   y++;
//   x = 0;
//   if( y > height) {
//     fb_roll();
//     y--;
//   }
//
//   terminal_cursor_update();
// }

void
terminal_cursor_update() {

}
