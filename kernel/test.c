
#include <debug/test.h>

void
test() {
    printf("------------------ test ------------------\n");
#if 1
    printf("COM1: 0x%x\n", bios_read_word(0x40,0x00));
    printf("COM2: 0x%x\n", bios_read_word(0x40,0x02));
    printf("COM3: 0x%x\n", bios_read_word(0x40,0x04));
    printf("COM4: 0x%x\n", bios_read_word(0x40,0x06));
#endif

#if 1
    bios_read_byte(0x20,0x00);
#endif

#if 1
    pci_test();
#endif

#if 0
    uint8_t tester = 1;
    tester = tester / 0;
#endif
    printf("--------------- end of test --------------\n");
}
