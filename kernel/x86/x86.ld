OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

VADDR_BASE = 0xC0000000; /* "high-half" kernel */
PADDR_BASE = 0x100000;

SECTIONS
{
   . = VADDR_BASE;
   kernel_vaddr_start = .;
   _kernel_vaddr_start = .;

   .text ALIGN (0x1000) : AT(ADDR(.text)-VADDR_BASE + PADDR_BASE)
   {
        kernel_paddr_start = .; _kernel_paddr_start = .;
       *(.text)
   }

   .rodata ALIGN (0x1000): AT(ADDR(.rodata)-VADDR_BASE + PADDR_BASE)
   {
       *(.rodata*)
   }

   .data ALIGN (0x1000) : AT(ADDR(.data)-VADDR_BASE + PADDR_BASE)
   {
       *(.data)
   }

   .bss ALIGN (0x1000) : AT(ADDR(.bss)-VADDR_BASE + PADDR_BASE)
   {
       *(.bss)
       *(COMMON)
   }
   kernel_vaddr_end = .;
   _kernel_vaddr_end = .;
 }
