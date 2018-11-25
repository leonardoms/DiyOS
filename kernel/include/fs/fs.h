#ifndef _FS_H
#define _FS_H

#include <kernel/types.h>
#include <kernel/mm.h>
#include <string.h>

// Tamanho do nome de um arquivo
#define FS_NAME_SZ  255
#define FS_MAGIC    0x65FACADA

// Tipos de nodo
#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08

// Tipos de acesso
#define	O_RDONLY	0x01
#define	O_WRONLY	0x02
#define	O_RDWR		(O_RDONLY | O_WRONLY)

struct dirent
{
    char *name;
    uint32_t ino;
};

struct _fs_node;

typedef uint32_t (*read_type_t)(struct _fs_node*,uint8_t*,uint32_t);
typedef uint32_t (*write_type_t)(struct _fs_node*,uint8_t*,uint32_t);
typedef void (*open_type_t)(struct _fs_node*, uint32_t mask);
typedef void (*close_type_t)(struct _fs_node*);
typedef uint32_t (*select_type_t)(struct _fs_node*,uint8_t, uint32_t*);
typedef struct dirent * (*readdir_type_t)(struct _fs_node*,uint32_t);
typedef struct _fs_node * (*finddir_type_t)(struct _fs_node*,char *name);

typedef struct _fs_node
{
    char            *name;
    uint32_t        mask;
    uint32_t        uid;
    uint32_t        gid;
    uint32_t        flags;
    uintptr_t       inode;
    uint32_t        length;
    uint32_t        impl;
    uint32_t        magic;
    uint32_t        offset;
//    size_t          pendent_nodes;      // Quantidade de nodos filhos. Para controle de deleção.
    read_type_t     read;
    write_type_t    write;
    open_type_t     open;
    close_type_t    close;
    select_type_t   select;
    readdir_type_t  readdir;
    finddir_type_t  finddir;

// Nodos de controle
// Precisa de todos eles?
    struct _fs_node *fs_parent_node;
    struct _fs_node *fs_child_nodes;
    struct _fs_node *fs_prior_node;
    struct _fs_node *fs_next_node;
} fs_node_t;

#ifndef _FS_C
extern fs_node_t *fs_root;
extern fs_node_t *fs_home;
#endif

// Função de root
fs_node_t *initialise_initrd(uintptr_t location, uintptr_t end_adr);

// Função para acesso dos nodos
uint32_t read_fs(fs_node_t *node, uint8_t *buffer, uint32_t size);
uint32_t write_fs(fs_node_t *node, uint8_t *buffer, uint32_t size);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);
fs_node_t *search_path(fs_node_t *node, char *path);

// Funções para linkagem de nodos
fs_node_t *fs_create_node(char*,uint32_t,fs_node_t*,read_type_t,write_type_t,open_type_t,close_type_t,readdir_type_t,finddir_type_t);
fs_node_t *fs_destroy_node(fs_node_t *fs_node);
fs_node_t *fs_clone_node(fs_node_t *fs_node);
fs_node_t *fs_node_index(fs_node_t *fs_node, int index);
fs_node_t *fs_link_node(fs_node_t *fs_node, fs_node_t *fs_new_node);
fs_node_t *fs_unlink_node(fs_node_t *fs_node);
void fs_node_setINode(fs_node_t *fs_node, uintptr_t inode);

// Incluindo controle de nodos dev
#include <fs/fsdev.h>

#endif
