
#include <kernel.h>

struct dirent*
readdir(const char* path, struct dirent* dir) {
  struct dirent* d;
  uint32_t offset = 0;

  if( dir != NULL ) {
    offset = dir->offset;
  }

  fs_node_t* node = finddir_fs(fs_root_get(), path);

  if( node ) {
    d = readdir_fs(node,offset);
    if( d == NULL )
      return NULL;
    d->offset = offset + 1;

    if( dir )
      free(dir);

    return d;
  }

  return NULL;
}
