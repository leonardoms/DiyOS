#include <kernel.h>

uint32_t
read(uint32_t fd, void* buffer, uint32_t size) {

  fs_node_t* file;
  task_t*    t;
  uint32_t  offset = 0; //TODO: get from file descriptor

  t = task_get();

  if( fd >= t->file_count)
    return 0;

  file = t->files[fd].node;

  return read_fs(file, offset, size, buffer);
}
