#include <stdio.h>
#include <stdlib.h>

// split file in path segments
uint8_t**
path(uint8_t* file) {
  uint32_t i = 0, i0 = 0, seg = 0, segs = 0;
  uint8_t** p;
  uint8_t* str;

  if( file == NULL )
    return NULL;

  if( file[i] == '/' )
    // return NULL;
    i0++;

  str = strdup(file);
  str = &str[i0]; // skip initial '/'
  segs = 1;
  while(str[i]) {
    if(str[i] == '/') {
      str[i] = '\0';
      if(str[i+1] == '\0')  // test for ending /
        break;
      segs++;
    }
    i++;
  }

  p = (uint8_t**)malloc(sizeof(uint8_t*) * (segs));

  for( seg = 0, i = 0; seg < segs; seg++ ) {
    p[seg] = &str[i];

    while( str[i++] );

  }

  p[seg] = NULL;

  // printf("path: %s (%d)\n", file, segs);


  return p;
}

void
path_free(uint8_t** p) {
  // free(p[0]);
  // free(p);
}
