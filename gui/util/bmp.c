#include <gui.h>

// read bmp files
bmp_image_t*
bmp_image_from_file(uint8_t* file) {
    uint32_t fd, sz, i;
    struct bmp_image hdr;
    bmp_image_t* bmp;

    fd = open(file,1,0);

    if( fd < 0 )
      return NULL;

    sz = read(fd, &hdr, sizeof(struct bmp_image));

    if( sz != sizeof(struct bmp_image) )
      return NULL;

    // printf("%s: %dkB %dx%dpx (%dbits), data @ +%d\n", file, hdr.total_size, hdr.width, hdr.height, hdr.bpp, hdr.offset);
    bmp = (bmp_image_t*)malloc(hdr.total_size);

    if( bmp == NULL ) {
      printf("bmp_image_from_file(): out of memory\n");
      return NULL;
    }

    sz = read(fd, bmp, hdr.total_size);

    if( sz != bmp->total_size ) {
      free(bmp);
      return NULL;
    }

    bmp->data = (uint8_t*)((uint32_t)bmp + bmp->offset);

    return bmp;
}
