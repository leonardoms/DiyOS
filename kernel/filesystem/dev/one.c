
#include <kernel.h>

uint32_t
dev_one_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {

  if( offset > size )
    return 0;

  memset(buffer, 1, size);

  return size;
}

void
dev_one() {
  fs_node_t* one;

  one = (fs_node_t*)malloc(sizeof(fs_node_t));
  strcpy(one->name, "one");
  one->flags = FS_FILE;
  one->read = &dev_one_read;
  one->write = 0;
  one->open = 0;
  one->close = 0;
  one->readdir = 0;
  one->finddir = 0;

  devfs_register(one);
}
