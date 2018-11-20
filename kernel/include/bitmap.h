#ifndef _BITMAP_H_
#define _BITMAP_H_

#define BITMAP_GET(b,p)   (((uint8_t*)b)[p/8] &   (1 << p%8))
#define BITMAP_SET(b,p)   (((uint8_t*)b)[p/8] |=  (1 << p%8))
#define BITMAP_UNSET(b,p) (((uint8_t*)b)[p/8] &= ~(1 << p%8))

#endif
