#include <stdlib.h>

char*
strcat(char* str1, char* str2) {
  char* s = (char*)malloc(strlen(str1) + strlen(str2) + 1);

  strcpy(s,str1);
  strcpy(&s[strlen(str1)],str2);

  return s;
}
