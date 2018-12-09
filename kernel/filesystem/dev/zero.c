
#include <kernel.h>

uint32_t
dev_zero_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {

  if( offset > size )
    return 0;

  memset(buffer, 0, size);

  return size;
}

void
dev_zero() {
  fs_node_t* zero;

  zero = (fs_node_t*)malloc(sizeof(fs_node_t));
  strcpy(zero->name, "zero");
  zero->flags = FS_FILE;
  zero->read = &dev_zero_read;
  zero->write = 0;
  zero->open = 0;
  zero->close = 0;
  zero->readdir = 0;
  zero->finddir = 0;

  devfs_register(zero, "zero");
}
