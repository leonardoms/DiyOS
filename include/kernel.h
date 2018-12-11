#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <arch.h>
#include <ring.h>


typedef struct console {
  ring_t  in, out;
} console_t;

#include <ctypes.h>
#include <kernel/aspace.h>
#include <kernel/bios.h>
#include <kernel/elf.h>
#include <kernel/fs.h>
#include <kernel/message.h>
#include <kernel/mm.h>
#include <kernel/panic.h>
#include <kernel/pci.h>
#include <kernel/pic.h>
#include <kernel/task.h>
#include <kernel/terminal.h>


void
sleep(uint32_t ms);

#endif
