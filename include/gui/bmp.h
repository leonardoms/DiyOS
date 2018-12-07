#ifndef _BMP_H_
#define _BMP_H_

#include <stdio.h>

#pragma pack(1)
typedef struct bmp_image {
  uint16_t  signature;
  uint32_t  total_size;
  uint16_t  reserved1;
  uint16_t  reserved2;
  uint32_t  offset;
  uint32_t  hdr_size;
  uint32_t  width;
  uint32_t  height;
  uint16_t  planes;
  uint16_t  bpp;
  uint32_t  compression;
  uint32_t  img_size;
  uint32_t  foo[4];
  uint8_t*  data;
} bmp_image_t;



#endif
