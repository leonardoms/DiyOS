
int strcmp(char *s1, char *s2){
  int   i;
  char *c1, *c2;
  for (c1=s1, c2=s2;(*c1)!='\0'; c1++,c2++){
    i = (*c1)-(*c2);
    if (i!=0){
      return i;
    }
  }
  if ((*c2)!=0){
    return -(*c2);
  }
  return 0;
}
