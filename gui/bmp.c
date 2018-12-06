#include <ctypes.h>

// read bmp files
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
} bmp_image_t;

bmp_image_t*
bmp_image_from_file(uint8_t* file) {
    uint32_t fd, sz, i;
    // uint8_t hdr[40];
    struct bmp_image hdr;

    fd = open(file,1,0);

    if( fd < 0 )
      return NULL;

    sz = read(fd, &hdr, sizeof(struct bmp_image));

    if( sz != sizeof(struct bmp_image) )
      return NULL;

    printf("%s: %dkB %dx%dpx (%dbits), data @ +%d\n", file, hdr.total_size/1024, hdr.width, hdr.height, hdr.bpp, hdr.offset);

    return NULL;
}
