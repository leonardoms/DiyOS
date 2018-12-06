
#include <list.h>

list_t*
list_add(list_t* l, void* data) {
  list_t *entry, *last;

  entry = (struct list*)malloc(sizeof(struct list));

  if( entry == NULL ) {
    printf("list_add: out of memory\n");
    return l;
  }

  entry->magic = LIST_MAGIC;
  entry->data = data;
  entry->next = NULL;

  if( l == NULL ) {
    return entry;
  } else {
    if( l->magic != LIST_MAGIC ) {
      printf("list_add: corromped! (0x%x)\n", l);
      return entry;
    }

    last = l;
    while( last->next )
      last = last->next;

    last->next = entry;

    return l;
  }

}

// if it() returns FALSE, then list_foreach() return the last iterated entry
list_t*
list_foreach(list_t* l, list_iterator_t it, void* udata) {
  list_t *entry;

  if( l == NULL )
    return NULL;

  if( l->magic != LIST_MAGIC ) {
    printf("list_foreach: corromped! (0x%x)\n", l);
    return NULL;
  }

  if( it == NULL )
    return NULL;

  entry = l;
  do {
    if( entry->magic != LIST_MAGIC ) {
      printf("list_foreach: corromped! (0x%x)\n", l);
      return;
    }

    if( it(entry, udata) == FALSE ) // iterate
      return entry;
  } while( (entry = entry->next) != NULL );

  return NULL;
}
