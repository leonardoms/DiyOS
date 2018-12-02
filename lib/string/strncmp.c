/************************************************
**Este c�digo � uma biblioteca do
**Prot�tipo de kernel do SOmBRA.
**
**
** SEM GARANTIAS
**
**Est� codigo n�o tem garantias, assim n�o
**me responsabilizo pelo que ele possa causar...
**
**Bom Proveito!
************************************************/

// Compara n elementos de s1 com s2.
int strncmp(const char *s1, const char *s2, unsigned int n){
  if (n == 0){
    return 0;
  }
  do{
    if (*s1 != *s2++){
      return *(unsigned const char *)s1 - *(unsigned const char *)--s2;
    }
    if (*s1++ == 0){
      break;
    }
  } while(--n!=0);

  return 0;
}
