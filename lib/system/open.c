
#include <kernel.h>

int32_t
open(uint8_t* name, uint8_t r, uint8_t w) {
  task_t* t;
  fs_node_t* file;

  fs_node_t* root = fs_root_get();

  t = task_get();

  if( (file = finddir_fs(root,name)) != NULL ) {
      t->files[t->file++].node = file;
      return (t->file - 1);
  } else
      return -1;
}
