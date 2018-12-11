#include <kernel.h>
#include <list.h>

static list_t* root_nodes = NULL;
static uint32_t root_files = 0;
fs_node_t *root_node = NULL; // The root of the filesystem.

static uint8_t
_fs_find(list_t* entry, void* udata) {
    if( !strcmp(((fs_node_t*)(entry->data))->name, (uint8_t*)udata) )
      return FALSE;
    else
      return TRUE;
}

fs_node_t*
root_fs_finddir(fs_node_t *node, char *name) {
  uint32_t i;
  list_t* entry;
  uint8_t** p;
  fs_node_t* i_node = NULL;

  // printf("find %s\n", name);

  if( name == NULL )
    return NULL;

  if( node == root_node ) {
    i_node = node;
    i = 0;

    p = path(name); // split directories

    if( p == NULL )
      return NULL;

    entry = list_foreach(root_nodes, _fs_find, p[i++]);
    if( entry ) {
      i_node = (fs_node_t*)entry->data;
    } else {
      path_free(p);
      return NULL;
    };

    while( p[i] ) {
      // printf("%s/?%s? ", i_node->name, p[i]);
      i_node = finddir_fs(i_node, p[i]);

      if( i_node == NULL ) {
        // printf("no\n");
        path_free(p);
        return NULL;
      } //else printf("yes\n");
      i++;
    }

  }

  if(p)
    path_free(p);

  return i_node;
}

struct dirent*
root_fs_readdir(fs_node_t *node, uint32_t index) {
    struct dirent* d = NULL;
    // fs_node_t* root_node;
    // list_t* entry;
    //
    // if( index >= root_files)
    //   return NULL;
    //
    // entry = list_get(root_nodes, index);
    // root_node = (fs_node_t*)entry->data;
    //
    // if( root_node ) {
    //   d = (struct dirent*)malloc(sizeof(struct dirent));
    //
    //   if( d == NULL )
    //     return NULL;
    //
    //   strcpy(d->name,root_node->name);
    //   d->inode = (uint32_t)root_node;
    // }

    return d;
}

void
fs_register(fs_node_t* node) {
  //TODO: check if node->name ahead exists

  root_nodes = list_add(root_nodes, node);

  root_files++;
}

fs_node_t*
fs_root_get() {
  return root_node;
}

fs_node_t*
fs_root() {
  root_node = (fs_node_t*)malloc(sizeof(fs_node_t));
  strcpy(root_node->name, "/");
  root_node->flags = FS_DIRECTORY;
  root_node->read = 0;
  root_node->write = 0;
  root_node->open = 0;
  root_node->close = 0;
  root_node->readdir = &root_fs_readdir;
  root_node->finddir = &root_fs_finddir;

  devfs();

  return root_node;
}
