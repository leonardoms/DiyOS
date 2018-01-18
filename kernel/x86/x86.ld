OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
 {
   . = 0x100000;
   .text : { *(.text) }
   .data : {
      . = ALIGN(4096);

      *(.data)
    }
   .irq : { . += 64; }
   .bss : { *(.bss)  }
 }
