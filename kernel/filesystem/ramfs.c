#include <kernel.h>

// RAMFS for .tar package file

// https://www.gnu.org/software/tar/manual/html_node/Standard.html

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

struct tar_header
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
};

uint32_t tar_getsize(const char *in);

fs_node_t *ramfs_root_node = NULL; // The ramfs mount point
fs_node_t *ramfs_nodes = NULL;
struct tar_header *headers[64]; // up to 64 files
static uint32_t files = 0;

fs_node_t*
ramfs_finddir(fs_node_t *node, char *name);

struct dirent*
ramfs_readdir(fs_node_t *node, uint32_t index);

uint32_t
ramfs_read(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);

fs_node_t*
ramfs_root() {
  return ramfs_root_node;
}

uint32_t
tar_parse(unsigned int address, uint32_t end);

fs_node_t*
ramfs(uint32_t address, uint32_t end) {
  uint32_t i;

  printf("RAMFS: %d files found.\n", tar_parse(address, end));

  ramfs_root_node = (fs_node_t*)malloc(sizeof(fs_node_t));
  strcpy(ramfs_root_node->name, "ram");
  ramfs_root_node->flags = FS_DIRECTORY;
  ramfs_root_node->read = 0;
  ramfs_root_node->write = 0;
  ramfs_root_node->open = 0;
  ramfs_root_node->close = 0;
  ramfs_root_node->readdir = &ramfs_readdir;
  ramfs_root_node->finddir = &ramfs_finddir;

  // initialize files nodes
  ramfs_nodes = (fs_node_t*)malloc(sizeof(fs_node_t) * files);
  for( i = 0; i < files; i++ ) {

    strcpy(ramfs_nodes[i].name, headers[i]->name);

    if( tar_getsize(headers[i]->size) == 0 ) {
      // for now, ignore subdirs
      ramfs_nodes[i].readdir = 0;//&ramfs_readdir;
      ramfs_nodes[i].finddir = 0;//&ramfs_finddir;
      ramfs_nodes[i].read  = 0;
      ramfs_nodes[i].flags = FS_DIRECTORY;
    } else {
      ramfs_nodes[i].readdir = 0;
      ramfs_nodes[i].finddir = 0;
      ramfs_nodes[i].read  = &ramfs_read;
      ramfs_nodes[i].flags = FS_FILE;
    }

    ramfs_nodes[i].write = 0;
    ramfs_nodes[i].open = 0;
    ramfs_nodes[i].close = 0;

    ramfs_nodes[i].inode = i;
  }

  return ramfs_root_node;
}

fs_node_t*
ramfs_finddir(fs_node_t *node, char *name) {
  uint32_t i;

  if( name == NULL )
    return NULL;

  if( node == ramfs_root_node ) {
    for( i = 0; i < files; i++ ) {
      if( !strcmp(ramfs_nodes[i].name, name) ) {
        return &ramfs_nodes[i]; // file found
      }
    }
  }

  return NULL;
}

struct dirent*
ramfs_readdir(fs_node_t *node, uint32_t index) {
    struct dirent* d;

    if( index >= files)
      return NULL;

    d = (struct dirent*)malloc(sizeof(struct dirent));

    if( d == NULL )
      return NULL;

    strcpy(d->name,ramfs_nodes[index].name);
    d->inode = ramfs_nodes[index].inode;

    return d;
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
