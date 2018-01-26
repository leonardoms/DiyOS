
#include <debug/test.h>

void
test() {
    printf("------------------ test ------------------\n");
#if 0
    printf("COM1: 0x%x\n", bios_read_word(0x40,0x00));
    printf("COM2: 0x%x\n", bios_read_word(0x40,0x02));
    printf("COM3: 0x%x\n", bios_read_word(0x40,0x04));
    printf("COM4: 0x%x\n", bios_read_word(0x40,0x06));
#endif
    printf("--------------- end of test --------------\n");
}
