bits 32

global kernel_stack

section .text
        ;multiboot spec for grub2
        align 4
        dd 0x1BADB002
        dd 0x00
        dd - (0x1BADB002 + 0x00)

extern do_it_yourself   ; void do_it_yourself(); from main.c

start:  ; for LD entrypoint
    cli
    mov     esp, kernel_stack
    call    do_it_yourself

    hlt
    jmp $

section .data


section .bss
    resb    8192
kernel_stack:
