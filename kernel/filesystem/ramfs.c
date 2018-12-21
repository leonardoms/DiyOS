#include <kernel.h>
#include <list.h>

// RAMFS for .tar package file

// https://www.gnu.org/software/tar/manual/html_node/Standard.html
// leo@debian:~/DiyOS_2019/ramdisk$ tar -H ustar -cvf ../bin/ramdisk.tar *


/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

#pragma pack(1)
struct tar_header
{
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[131];             /* 345 */
  char atime[12];               /* 476 */
  char ctime[12];               /* 488 */
};

uint32_t tar_getsize(const char *in);

fs_node_t *ramfs_root_node = NULL; // The ramfs mount point
list_t* ramfs_nodes = NULL;
struct tar_header *headers[64]; // up to 64 files
static uint32_t files = 0;

fs_node_t*
ramfs_finddir(fs_node_t *node, char *name);

struct dirent*
ramfs_readdir(fs_node_t *node, uint32_t index);

uint32_t
ramfs_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);

uint32_t
tar_parse(unsigned int address, uint32_t end);

static uint8_t
_find_name(list_t* entry, void* udata) {
    if( !strcmp(((fs_node_t*)(entry->data))->name, (uint8_t*)udata) )
      return FALSE;
    else
      return TRUE;
}

fs_node_t*
ramfs_root() {
  return ramfs_root_node;
}

fs_node_t*
_ramfs_create_path(uint8_t** p) {
  // - if segments == 1, do nothing and returns ramfs_root_node
  fs_node_t *parent, *node;
  uint32_t s = 0;
  list_t* entry;

  if( p == NULL )
    return ramfs_root_node;

  parent = ramfs_root_node;

  // uint8_t i = 0;
  // while(p[i]) {
  //   printf("[%x] ", &p[i++]);
  // }
  // printf("\n");

  while( p[s+1] != NULL) { // [s+1] -> ignore last segment
    // try to find child

    entry = list_foreach((list_t*)parent->inode,_find_name,p[s]);
    if( entry ) {
      parent = (fs_node_t*)entry->data;
    } else {
      node = (fs_node_t*)malloc(sizeof(fs_node_t));
      strcpy(node->name, p[s]);
      // printf("create %s/%s \n", parent->name, p[s]);
      node->inode = 0;  // if has child, when child added will be the ptr for the list_t
      node->flags = FS_DIRECTORY;
      node->read = 0;
      node->write = 0;
      node->open = 0;
      node->close = 0;
      node->readdir = &ramfs_readdir;
      node->finddir = &ramfs_finddir;

      parent->inode = (uint32_t)list_add((list_t*)parent->inode,node);

      parent = node;
    }

    s++;
  }

  return parent;
}

fs_node_t*
ramfs(uint32_t address, uint32_t end) {
  uint32_t i, f = 0;

  tar_parse(address, end);

  ramfs_root_node = (fs_node_t*)malloc(sizeof(fs_node_t));
  strcpy(ramfs_root_node->name, "ram");
  ramfs_root_node->flags = FS_DIRECTORY;
  ramfs_root_node->read = 0;
  ramfs_root_node->write = 0;
  ramfs_root_node->open = 0;
  ramfs_root_node->close = 0;
  ramfs_root_node->readdir = &ramfs_readdir;
  ramfs_root_node->finddir = &ramfs_finddir;
  ramfs_root_node->inode = 0;

  uint8_t s = 0;
  fs_node_t* p_node, *node;
  uint8_t **p;

  for( i = 0; i < files; i++ ) {
    // printf("%s\n", headers[i]->name);

    p = path(headers[i]->name);

    p_node = _ramfs_create_path(p);

    s = 0;
    while(p[s++]);
    s--;

    switch (headers[i]->typeflag) {
      case REGTYPE:
      case AREGTYPE:

        node = (fs_node_t*)malloc(sizeof(fs_node_t));

        strcpy(node->name, p[s-1]);
        node->inode = i;
        node->flags = FS_FILE;
        node->read = &ramfs_read;
        node->write = 0;
        node->open = 0;
        node->close = 0;
        node->readdir = 0;
        node->finddir = 0;

        f++;

        break;
      case DIRTYPE:

        p_node = _ramfs_create_path(p);

        node = (fs_node_t*)malloc(sizeof(fs_node_t));

        strcpy(node->name, p[s-1]);
        node->inode = 0;  // if has child, when child added will be the ptr for the list_t
        node->flags = FS_DIRECTORY;
        node->read = 0;
        node->write = 0;
        node->open = 0;
        node->close = 0;
        node->readdir = &ramfs_readdir;
        node->finddir = &ramfs_finddir;

        break;
      default:
        printf("ramdisk: '%s' type (%c) not suported.\n", headers[i]->name, headers[i]->typeflag);
        break;
    }

    // printf("add %s to %s parent\n", node->name, p_node->name);
    p_node->inode = (uint32_t)list_add((list_t*)p_node->inode, node);
    path_free(p);
  }

  // ramfs_root_node->inode = (uint32_t)ramfs_nodes;

  fs_register(ramfs_root_node, "ram"); // register as /ram

  printf("/ram filled with %d file(s).\n", f);

  return ramfs_root_node;
}

fs_node_t*
ramfs_finddir(fs_node_t *node, char *name) {
  uint32_t i;
  list_t  *nodes_list, *entry;

  if( name == NULL )
    return NULL;


  if( node->flags & FS_DIRECTORY ) {
    nodes_list = (list_t*)node->inode;
    entry = list_foreach(nodes_list,_find_name,name);
    if( entry )
      return (fs_node_t*)entry->data;
  }

  return NULL;
}

struct dirent*
ramfs_readdir(fs_node_t *node, uint32_t index) {
    struct dirent* d;
    fs_node_t*  i_node;
    list_t* nodes_list, *entry;

    if( node == NULL )
      return NULL;

    if( node->flags & FS_DIRECTORY ) {

      nodes_list = (list_t*)node->inode;

      entry = list_get(nodes_list,index);

      if( entry == NULL )
        return NULL;

      i_node = (fs_node_t*)entry->data;

      d = (struct dirent*)malloc(sizeof(struct dirent));

      if( d == NULL )
        return NULL;

      strcpy(d->name,i_node->name);
      d->inode = i_node->inode;
      d->offset = index;
      d->flags = i_node->flags;

      return d;
    }

    return NULL;
}

uint32_t
ramfs_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
  uint32_t file_sz;

  file_sz = tar_getsize(headers[node->inode]->size);

  if( offset > file_sz )
    return 0;

  if( (offset + size) > file_sz )
    size = file_sz - offset;

  memcpy( (uint8_t*)((uint32_t)headers[node->inode] + 512), buffer, size);

  // dump_tar_file(headers[node->inode]);

  return size;
}

uint32_t
tar_getsize(const char *in) {

    uint32_t size = 0;
    uint32_t j;
    uint32_t count = 1;

    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);

    return size;
}

uint32_t
tar_parse(unsigned int address, uint32_t end)
{

    for (files = 0; ; files++) {

        struct tar_header *header = (struct tar_header*)address;

        if (header->name[0] == '\0')
            break;

        uint32_t size = tar_getsize(header->size);

        headers[files] = header;

        address += ((size / 512) + 1) * 512;

        if (size % 512)
            address += 512;

        if( address > end )
          break;

        // printf("-> %s ", headers[files]->name);
        // // if(headers[files]->typeflag == DIRTYPE)
        // if(size == 0)
        //   printf("(directory)\n");
        // else
        // // if(headers[files]->typeflag == REGTYPE)
        //   printf("(%d bytes)\n", size);
    }

    return files;

}

void
dump_tar_file(struct tar_header* tar) {
    uint8_t content[9];

    memcpy((uint8_t*)((uint32_t)tar + 512), content, 8);
    content[8] = '\0';

    printf("%s: sz=%d, flag=%c, content=[%s...]\n", tar->name, tar_getsize(tar->size), tar->typeflag, content);
}
