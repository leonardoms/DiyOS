#ifndef _MOUSE_H_
#define _MOUSE_H_

#include <kernel.h>
#include <ctypes.h>
#include <queue.h>

#define MOUSE_LEFT    1
#define MOUSE_RIGHT   2
#define MOUSE_MIDLE   4

struct mouse_packet {
    uint8_t flags;
    int8_t dx;
    int8_t dy;
} __attribute__ ((packed));

void
mouse(void);

#endif
