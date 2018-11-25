#ifndef CONDEV_H
#define CONDEV_H

#include <kernel.h>

#define	O_RDONLY	0x01
#define	O_WRONLY	0x02
#define	O_RDWR		(O_RDONLY | O_WRONLY)

int open_stdcon(file_t *f);
int select_con(file_t *f, unsigned access, unsigned *timeout);
int read_con(file_t *f, unsigned char *buf, unsigned len);
int write_con(file_t *f, unsigned char *buf, unsigned len);
int close_con(file_t *f);
int open_con(file_t *f);

#endif
