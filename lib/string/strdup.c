
#include <stdlib.h>

uint8_t*
strdup(uint8_t* str) {
  uint8_t* new_str;

  if( str == NULL )
    return NULL;

  new_str = (uint8_t*)malloc(strlen(str)+1);

  if(new_str == NULL)
    return NULL;

  strcpy(new_str,str);

  return new_str;
}
