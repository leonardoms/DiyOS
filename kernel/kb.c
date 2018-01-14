// keyboard built-in
#include "kb.h"

void keyboard_handle() {
    uint8_t c = inb(0x60);
    if(c == 0x1)
      fb_puts("System is halted...\n"); // nothing important.. just a test
    if(c == 10)
      fb_newline();
}

void setup_kb() {
    irq_install(0x21, keyboard_handle);
}
