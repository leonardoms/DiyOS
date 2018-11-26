#include <fs.h>

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

fs_node_t *fs_ramfs = 0; // The ramfs mount point

struct tar_header *headers[64]; // up to 64 files
static uint32_t files = 0;

fs_node_t*
ramfs_node() {
  return fs_ramfs;
}

uint32_t
tar_parse(unsigned int address, uint32_t end);

fs_node_t*
ramfs(uint32_t address, uint32_t end) {
  printf("RAMFS: %d files found.\n", tar_parse(address, end));
  // TODO: create iNodes for files!

  return fs_ramfs;
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

        printf("-> %s ", headers[files]->name);
        // if(headers[files]->typeflag == DIRTYPE)
        if(size == 0)
          printf("(directory)\n");
        else
        // if(headers[files]->typeflag == REGTYPE)
          printf("(%d bytes)\n", size);
    }

    return files;

}
