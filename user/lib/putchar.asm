
global  putchar

putchar:
    mov   ebx, [esp + 4]
    mov   eax, 0x1              ; syscall write
    int   0x42
    ret
