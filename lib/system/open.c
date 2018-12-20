
#include <kernel.h>

int32_t
open(uint8_t* name, uint8_t r, uint8_t w) {
  task_t* t;
  fs_node_t* file;

  fs_node_t* root = fs_root_get();

  t = task_get();

  if( (file = finddir_fs(root,name)) != NULL ) {
      t->file_count++;
      t->files[t->file_count - 1].node = file;
      return (t->file_count - 1);
  } else
      return -1;
}
