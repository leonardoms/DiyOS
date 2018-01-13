#ifndef _X86_H_
#define _X86_H_

#include "ctypes.h"

struct gdt {
  uint16_t size;
  uint32_t offset;
} __attribute__ ((__packed__));

struct gdt_entry {
    uint32_t low;
    uint32_t high;
} __attribute__ ((aligned (8)));

void setup_gdt();
void setup_idt();

#endif
