#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <ctypes.h>

#define FC_BLACK      0x0
#define FC_BLUE       0x1
#define FC_GREEN      0x2
#define FC_CYAN       0x3
#define FC_RED        0x4
#define FC_MAGENTA    0x5
#define FC_BROWN      0x6
#define FC_YELLOW     0xE
#define FC_WHITE      0xF
#define FC_DGRAY      0x8
#define FC_LGRAY      0x7
#define FC_LBLUE      0x9
#define FC_LGREEN     0xA
#define FC_LCYAN      0xB
#define FC_LRED       0xC
#define FC_LMAGENTA   0xD
#define FC_GRAY       FC_DGRAY

#define BC_BLACK      0x0
#define BC_BLUE       0x10
#define BC_GREEN      0x20
#define BC_CYAN       0x30
#define BC_RED        0x40
#define BC_MAGENTA    0x50
#define BC_BROWN      0x60
#define BC_YELLOW     0xE0
#define BC_WHITE      0xF0
#define BC_GRAY       0x70

#define BLINK_TEXT    0x80

typedef struct terminal {
  uint8_t   id;
  uint8_t*  buffer;
  uint8_t   rows, cols;
  uint8_t   row, col;
  uint8_t   color;
} terminal_t;

terminal_t*
terminal(void);

void
terminal_goto(terminal_t* term, uint8_t _row, uint8_t _col);

void
terminal_color(terminal_t* term, uint8_t _color);

void
terminal_getpos(terminal_t* term, uint8_t* _row, uint8_t* _col);

void
terminal_get_color(terminal_t* term, uint8_t* _color);

#endif
