
#include <kernel.h>

int32_t
open(uint8_t* name, uint8_t r, uint8_t w) {
  task_t* t;
  fs_node_t* file;

  fs_node_t* ram = ramfs_root();

  t = task_get();

  if( (file = finddir_fs(ram,name)) != NULL ) {
      t->files[t->file++] = file;
      return (t->file - 1);
  } else
      return -1;
}
