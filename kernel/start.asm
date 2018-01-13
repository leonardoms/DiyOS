bits 32

global kernel_stack
global load_gdt

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

load_gdt:
    cli
    mov   eax, [esp + 4]
    lgdt  [eax]
    
    jmp 0x08:reload_segment    ; 0x08 = kernel code segment
 reload_segment:               ; far jump make CS = 0x08
    mov  ax, 0x10              ; 0x10 = kernel data segment
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax
    mov  ss, ax
    ret

section .data


section .bss
    resb    8192
kernel_stack:
