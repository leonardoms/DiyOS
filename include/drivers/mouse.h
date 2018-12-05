#ifndef _MOUSE_H_
#define _MOUSE_H_

#include <kernel.h>
#include <ctypes.h>
#include <queue.h>


struct mouse_packet {
    uint8_t flags;
    uint8_t dx;
    uint8_t dy;
};

void
mouse(void);

#endif
