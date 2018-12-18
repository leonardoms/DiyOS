#include <kernel.h>

void
panic(const char *message, const char *file, uint32_t line) {
    __asm__ __volatile__ ("cli\n");
    printf("\nPANIC: %s. %s:%d\n", message, file, line);
    while(1);
}

void
panic_assert(const char *file, uint32_t line, const char *desc) {
    __asm__ __volatile__ ("cli\n");
    printf("\nPANIC: (%s) is false. %s:%d\n", desc, file, line);
    while(1);
}
