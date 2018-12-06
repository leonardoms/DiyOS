#include <ctypes.h>

// read bmp files
typedef struct bmp {
  uint8_t   signature[2];
  uint32_t  total_size;
  uint32_t  reserved1;
  uint32_t  offset;
  uint32_t  hdr_size;
  uint32_t  width;
  uint32_t  height;
  uint16_t  planes;
  uint16_t  bpp;
  uint32_t  compression;
  uint32_t  img_size;
  /* ... */
} bmp_t;
