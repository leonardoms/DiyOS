OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

/* 0x000000 ~ 0x100000 = reserved area for BIOS, multiboot, etc.
/* 0x100000 ~ 0x200000 = reserved area for page tables */
PADDR_BASE = 0x200000;
VADDR_BASE = 0xC0000000; /* "high-half" kernel */

SECTIONS
{
   . = VADDR_BASE;
   kernel_vaddr_start = .;
   _kernel_vaddr_start = .;

   .text ALIGN (0x1000) : AT(PADDR_BASE) /* load code at 0x200000 physical memory */
   {
        kernel_paddr_start = .; _kernel_paddr_start = .;
       *(.text)
   }

   .rodata ALIGN (0x1000):
   {
       *(.rodata*)
   }

   .data ALIGN (0x1000) :
   {
       *(.data)
   }

   .bss ALIGN (0x1000) :
   {
       *(.bss)
       *(COMMON)
   }
   kernel_vaddr_end = .;
   _kernel_vaddr_end = .;
 }
