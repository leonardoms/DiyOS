// thanks http://www.jamesmolloy.co.uk ;)

#include <kernel.h>

uint32_t
read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    // Has the node got a read callback?
    if (node->read != 0)
        return node->read(node, offset, size, buffer);
    else
        return 0;
}

uint32_t
write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    // Has the node got a write callback?
    if (node->write != 0)
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void
open_fs(fs_node_t *node, uint8_t read, uint8_t write)
{
    // Has the node got an open callback?
    if (node->open != 0)
        return node->open(node);
}

void
close_fs(fs_node_t *node)
{
    // Has the node got a close callback?
    if (node->close != 0)
        return node->close(node);
}

char*
readdir_fs(fs_node_t *node, uint32_t index)
{
    // Is the node a directory, and does it have a callback?
    if ( (node->flags & 0x7) == FS_DIRECTORY &&
         node->readdir != 0 )
        return node->readdir(node, index);
    else
        return 0;
}

fs_node_t*
finddir_fs(fs_node_t *node, char *name)
{
    // Is the node a directory, and does it have a callback?
    if ( (node->flags & 0x7) == FS_DIRECTORY &&
         node->finddir != 0 )
        return node->finddir(node, name);
    else
        return 0;
}
