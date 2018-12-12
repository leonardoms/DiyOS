
#include <kernel.h>

uint32_t
dev_rand_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
  uint32_t i;

  if( offset > size )
    return 0;

  srand((uint32_t)&buffer);

  for( i = 0; i < size; i++ ) {
    buffer[i] = rand();
    srand(buffer[i]);
  }

  return size;
}

void
dev_rand() {
  fs_node_t* rand;

  rand = (fs_node_t*)malloc(sizeof(fs_node_t));
  strcpy(rand->name, "random");
  rand->flags = FS_FILE;
  rand->read = &dev_rand_read;
  rand->write = 0;
  rand->open = 0;
  rand->close = 0;
  rand->readdir = 0;
  rand->finddir = 0;

  devfs_register(rand);
}
