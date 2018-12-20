#include <kernel.h>
#include <list.h>

static list_t* dev_nodes = NULL;
static uint32_t dev_files = 0;
static fs_node_t* devfs_root_node = NULL;

static uint8_t
_devfs_find(list_t* entry, void* udata) {
    if( !strcmp(((fs_node_t*)(entry->data))->name, (uint8_t*)udata) )
      return FALSE;
    else
      return TRUE;
}

fs_node_t*
devfs_finddir(fs_node_t *node, char *name) {
  uint32_t i;
  list_t* entry;
  fs_node_t* dev_node = NULL;

  if( name == NULL )
    return NULL;

  if( node == devfs_root_node ) {
    entry = list_foreach(dev_nodes, _devfs_find, name);
    if( entry ) {
      dev_node = (fs_node_t*)entry->data;
    }

  }

  return dev_node;
}

struct dirent*
devfs_readdir(fs_node_t *node, uint32_t index) {
    struct dirent* d = NULL;
    fs_node_t* dev_node;
    list_t* entry;

    if( index >= dev_files)
      return NULL;

    entry = list_get(dev_nodes, index);
    dev_node = (fs_node_t*)entry->data;

    if( dev_node ) {
      d = (struct dirent*)malloc(sizeof(struct dirent));

      if( d == NULL )
        return NULL;

      strcpy(d->name,dev_node->name);
      d->inode = (uint32_t)dev_node;
      d->offset = index;
    }

    return d;
}

void
devfs_register(fs_node_t* node) {
  //TODO: check if node->name ahead exists

  dev_nodes = list_add(dev_nodes, node);
  printf("devfs: '/dev/%s' created.\n", node->name);
  dev_files++;
}

fs_node_t*
devfs() {

  devfs_root_node = (fs_node_t*)malloc(sizeof(fs_node_t));
  strcpy(devfs_root_node->name, "dev");
  devfs_root_node->flags = FS_DIRECTORY;
  devfs_root_node->read = 0;
  devfs_root_node->write = 0;
  devfs_root_node->open = 0;
  devfs_root_node->close = 0;
  devfs_root_node->readdir = &devfs_readdir;
  devfs_root_node->finddir = &devfs_finddir;

  dev_one();
  dev_zero();
  dev_rand();
  dev_tty();

  printf("/dev initialized.\n");

  fs_register(devfs_root_node);

  return devfs_root_node;
}
