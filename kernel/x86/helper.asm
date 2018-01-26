global load_gdt
global load_idt
global load_pagedir

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

load_pagedir:
    mov   eax, [esp + 4]
    mov   cr3, eax
    mov   eax, cr0
    or    eax, 0x80000001
    mov   cr0, eax
    lea   ebx, [reload_mem]
    jmp   ebx
reload_mem:
    ret
