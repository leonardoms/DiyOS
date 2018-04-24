extern main

section .text
    call main

    mov   eax, 1    ; write function
    mov   ebx, 85   ; 'U' char
    int   0x42      ; write 'U' on screen

    mov   eax, 0xAABBCCDD ; just for debug

    jmp   $  ; replace by sysexit
