global load_gdt
global load_idt
global load_tss
global load_pagedir
global to_user
global user_sti
global to_kernel

user_sti:
    pushf
    pop   eax
    or    eax, 0x200
    push  eax
    popf
    ret

load_gdt:
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

load_idt:
    mov   eax, [esp + 4]
    lidt  [eax]
    ret

load_tss:
    mov   ax, 0x2B
    ltr   ax
    ret

to_user:
    cli

    pushf
    pop   ebx
    or    ebx, 0x200  ; IF = 1

    mov   ax, 0x23
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax

    mov   eax, [esp + 4]
    push  0x23         ; SS
    push  0xBF8FFFFB   ; ESP
    push  ebx          ; EFLAGS
    push  0x1B        ; CS
    push  eax         ; EIP
    ;xchg  bx, bx      ; Kernel Stack with User values ---^
    iret              ; force load User segments/registers
