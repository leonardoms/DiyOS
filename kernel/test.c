
#include <debug/test.h>
#include <debug/bochs.h>

void
test() {
    printf("------------------ test ------------------\n");
#if 0
    printf("COM1: 0x%x\n", bios_read_word(0x40,0x00));
    printf("COM2: 0x%x\n", bios_read_word(0x40,0x02));
    printf("COM3: 0x%x\n", bios_read_word(0x40,0x04));
    printf("COM4: 0x%x\n", bios_read_word(0x40,0x06));
    bios_read_byte(0x20,0x00);
#endif

#if 0
    pci_test();
#endif

#if 0
    uint8_t tester = 1;
    tester = tester / 0;
#endif

#if 1
    uint32_t i;
    uint8_t* my_dynamic_data = (uint8_t*)kmalloc(15000);

    printf("0x%x = kmalloc(1024)\n", (uint32_t)kmalloc(1024));
    printf("0x%x = kmalloc(4000)\n", (uint32_t)kmalloc(4000));
    printf("0x%x = kmalloc(10240)\n", (uint32_t)kmalloc(10240));
    printf("0x%x = kmalloc(5)\n", (uint32_t)kmalloc(5));
    printf("0x%x = kmalloc(15000)\n", (uint32_t)my_dynamic_data);
    printf("0x%x = kmalloc(8192)\n", (uint32_t)kmalloc(9000));

BOCHS_BREAKPOINT

    memory_debug_addr((uint32_t)kmalloc(10000));
    memory_debug_addr(0xDEADBEEF);
    for(i=0; i < 15000; i++)
       my_dynamic_data[0] = 0xDA;
#endif
    printf("--------------- end of test --------------\n");
}
