bits 32

global start
global kernel_stack
global page_directory
extern do_it_yourself   ; void do_it_yourself(); from main.c

; from LD script
;extern kernel_paddr_start   ; physical address base
;extern kernel_vaddr_start   ; virtual address base


section .text
        ;multiboot spec for grub2
        align 4
        dd 0x1BADB002
        dd 0x00
        dd - (0x1BADB002 + 0x00)

start:  ; for LD entrypoint
    cli

    mov     esp, kernel_stack

    ; temporary paging
    ;
    ; setup 0..4MB identity paging
    mov     eax, 0x0
    mov     ebx, 0x0
    .fill_table0:
         mov    ecx, ebx
         or     ecx, 3              ; Present; Supervisor;
         mov    [page_table0-0xC0000000+0x100000+eax*4], ecx
         add    ebx, 4096
         inc    eax
         cmp    eax, 1024            ; 1024 frames = 4MB
         je     .end0
         jmp    .fill_table0
    .end0:

    ; maps 0xC0000000 virtual to 0x100000 physical
    mov     eax, 0x0
    mov     ebx, 0x100000
    .fill_table:
         mov    ecx, ebx
         or     ecx, 3              ; Present; Supervisor; R/W;
         mov    [page_table768-0xC0000000+0x100000+eax*4], ecx
         add    ebx, 4096
         inc    eax
         cmp    eax, 1024
         je     .end
         jmp    .fill_table
    .end:

    mov     eax, page_table0
    sub     eax, 0xC0000000              ; translate to
    add     eax, 0x100000                ; physical
    and     eax, 0xFFFFF000
    or      eax, 3
    mov     ebx, page_directory
    sub     ebx, 0xC0000000              ; translate to
    add     ebx, 0x100000              ; physical
    mov     [ebx], eax

    mov     eax, page_table768
    sub     eax, 0xC0000000
    add     eax, 0x100000
    and     eax, 0xFFFFF000
    or      eax, 3
    mov     ebx, page_directory
    sub     ebx, 0xC0000000              ; translate to
    add     ebx, 0x100000              ; physical
    mov     [ebx+768*4], eax             ; 768º Dir = 0xC0000000 TODO: calculate index

    mov     eax, page_directory
    and     eax, 0xFFFFF000
    sub     eax, 0xC0000000
    add     eax, 0x100000
    or      eax, 3
    mov     cr3, eax
    xchg    bx, bx
    mov     eax, cr0
    or      eax, 0x80000001

    mov     cr0, eax

    lea     ebx, [go_to_virtual]
    jmp     ebx

go_to_virtual:

    ; go to C code
    call    do_it_yourself

    hlt
    jmp $

section .data


section .bss
align 0x1000

page_directory:
    resb  0x1000
page_table0:
    resb  0x1000
page_table768:
    resb  0x1000

kernel_stack_end:
    resb    8192
kernel_stack:
