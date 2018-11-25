#ifndef _FSDEV_H
#define _FSDEV_H

#include <fs/fs.h>
#include <kernel/types.h>

// Estrutura para armazenar disposítivos
typedef struct _dev_disp
{
    char *name;
    struct _dev_disp *dev_next_disp;
} dev_disp_t;

fs_node_t *dev_init_node(fs_node_t *fs_node);

#endif
