#include <gui.h>

unsigned char
reverse_bits(unsigned char num)
{
    unsigned char  NO_OF_BITS = 8;
    unsigned char reverse_num = 0, i, temp;

    for (i = 0; i < NO_OF_BITS; i++)
    {
        temp = (num & (1 << i));
        if(temp)
            reverse_num |= (1 << ((NO_OF_BITS - 1) - i));
    }

    return reverse_num;
}

// read bmp files
bmp_image_t*
bmp_image_from_file(uint8_t* file) {
    uint32_t fd, sz, i;
    struct bmp_image hdr;
    bmp_image_t* bmp;
    uint8_t dot;

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

    if( hdr.height > 0 ) {  // upside down image

   /*   uint32_t i, j, d, bpp;

      bpp = (hdr.bpp >> 3);

      for( i = 0; i < hdr.height / 2; i++ )
      for( j = 0; j < hdr.width; j++ ) {
        // debug_printf("(%d,%d)<>(%d,%d)\n", i, j, (hdr.height-i), (hdr.width-j));
        for( d = 0; d < hdr.bpp >> 3; d++) {
          dot = bmp->data[(i*hdr.width+j)*bpp+d];
          bmp->data[(i*hdr.width+j)*bpp+d] = reverse_bits(bmp->data[((hdr.height-i)*hdr.width+j)*bpp+(bpp-d-1)]);
          bmp->data[((hdr.height-i)*hdr.width+j)*bpp+(bpp-d-1)] = reverse_bits(dot);
        }
      }*/

    } else hdr.height = -hdr.height;

    debug_printf("done(0x%x)\n", (uint32_t)bmp);

    return bmp;
}
