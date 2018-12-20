#include <gui.h>

uint8_t
point_is_inside(struct widget* widget, int32_t x, int32_t y) {

  if( widget != NULL )
  if( widget->x <= x )
  if( widget->y <= y )
  if( (widget->x +  WIDGET(widget)->w) >= x )
  if( (widget->y +  WIDGET(widget)->h) >= y )
    return 1;

  // debug_printf("mouse outside. (%d,%d) > (%d,%d,%d,%d)\n", x, y, widget->x, widget->y, (widget->x +  WIDGET(widget)->w), (widget->y +  WIDGET(widget)->h));

  return 0;
}
