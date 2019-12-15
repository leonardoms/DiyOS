#include <gui.h>

// read bmp files
bmp_image_t*
bmp_image_from_file(uint8_t* file) {
    uint32_t fd, sz, i;
    struct bmp_image hdr;
    bmp_image_t* bmp;

    debug_printf("bmp_image_from_file(): '%s' ... ", file);
    fd = open(file,1,0);

    if( fd < 0 )
      return NULL;

    sz = read(fd, &hdr, sizeof(struct bmp_image));

    if( sz != sizeof(struct bmp_image) )
      return NULL;

    // debug_printf("%s: %dkB %dx%dpx (%dbits), data @ +%d\n", file, hdr.total_size, hdr.width, hdr.height, hdr.bpp, hdr.offset);
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

    // if( hdr.height > 0 ) {  // upside down image
    //
    //   uint8_t* tmp = (uint8_t*)malloc(hdr.width * hdr.bpp >> 3);
    //
    //   if( tmp == NULL ) {
    //     printf("bmp_image_from_file(): out of memory\n");
    //     free(bmp);
    //     return NULL;
    //   }
    // 
    //   uint32_t i;
    //   for( i = 0; i < (hdr.height - 1); i++ )
    //   memcpy( &bmp->data[(hdr.width * hdr.bpp >> 3) + i + 1]),
    //           &bmp->data[(hdr.width * hdr.bpp >> 3) + i]),
    //           (hdr.width * hdr.bpp >> 3) );
    //
    // }

    debug_printf("done(0x%x)\n", (uint32_t)bmp);

    return bmp;
}
