global isr_set_handler
global isr_get_handler
global load_idt

extern isr_call_ptr_vector
extern isr_fault

isr_handler:
    xchg    bx, bx
    push    gs
    push    fs
    push    es
    push    ds
    pusha

    mov     ax, 0x10        ; kernel data segment
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    cld

    push    esp                    ; pointer for int_regs_t
    mov     ebp, esp
    mov     ebx, [ebp + 52]       ; int number
    mov     eax, [isr_call_ptr_vector]

    mov     eax, [eax + ebx * 4]  ; callback for isr#
    cmp     eax, dword 0          ; callback is null ?
    je      __isr_ptr_null

    call    eax             ; run the callback

    jmp     isr_common

__isr_ptr_null:
    call    isr_fault       ; handle with a default message ...


isr_common:
    add esp, 4              ; remove int_regs_t* from stack

    popa
    pop     ds
    pop     es
    pop     fs
    pop     gs

    add     esp, 8          ; error code / isr number
    iret

isr_get_handler:
    push    ebp
    mov     ebp, esp
    push    eax
    push    esi

    mov     esi, [ebp + 12]

    xor     ebx, ebx
    xor     eax, eax
    mov     bl, [ebp + 8]
    shl     ebx, 3
    mov     al, [_idt + ebx + 5]
    mov     [esi], eax

    mov     eax, [isr_call_ptr_vector]
    mov     ebx, [ebp + 8]
    mov     esi, [eax + ebx * 4]
    mov     [esi + 4], esi

    pop     esi
    pop     eax
    pop     ebp
    ret

isr_set_handler:
    push    ebp
    mov     ebp, esp
    push    eax
    push    esi

    mov     esi, [ebp + 12]

    mov     eax, [esi]
    xor     ebx,ebx
    mov     bl, [ebp + 8]
    shl     ebx,3
    mov     [_idt + ebx + 5], al

    mov     eax, [isr_call_ptr_vector]
    mov     ebx, [ebp + 8]
    mov     esi, [esi + 4]
    mov     [eax + ebx * 4], esi

    pop     esi
    pop     eax
    pop     ebp
    ret

[section .data]

%macro PUSHB 1
	db 6Ah
	db %1
%endmacro

%macro INTR 1
global isr%1
isr%1:
    cli
	push byte 0
	PUSHB %1
	jmp near isr_handler
%endmacro

%macro INTR_EC 1
global isr%1
isr%1:
  cli
	nop
	nop
	PUSHB %1
	jmp near isr_handler
%endmacro

INTR 0
INTR 1
INTR 2
INTR 3
INTR 4
INTR 5
INTR 6
INTR 7
INTR_EC 8
INTR 9
INTR_EC 0Ah
INTR_EC 0Bh
INTR_EC 0Ch
INTR_EC 0Dh
INTR_EC 0Eh
INTR 0Fh
INTR 10h
INTR 11h
INTR 12h
INTR 13h
INTR 14h
INTR 15h
INTR 16h
INTR 17h
INTR 18h
INTR 19h
INTR 1Ah
INTR 1Bh
INTR 1Ch
INTR 1Dh
INTR 1Eh
INTR 1Fh

INTR 20h
INTR 21h
INTR 22h
INTR 23h
INTR 24h
INTR 25h
INTR 26h
INTR 27h
INTR 28h
INTR 29h
INTR 2Ah
INTR 2Bh
INTR 2Ch
INTR 2Dh
INTR 2Eh
INTR 2Fh

%assign i 30h
%rep (0FFh - 2Fh)

	INTR i

%assign i (i + 1)
%endrep

_idt:
%rep 256
  	dw     0
  	dw     0x8
  	db     0
  	db     0x8E
  	dw     0
%endrep
_idt_end:

[section .text]

idt_ptr:
  	dw     _idt_end - _idt - 1
  	dd     _idt


    load_idt:
        mov ecx, ((_idt_end - _idt) / 8)
        mov ebx, _idt
        mov edx, isr0
    build_idt:
        mov eax, edx
        mov [ebx], ax
        shr eax, 16
        mov [ebx + 6], ax
        add ebx, byte 8
        add edx, byte (isr1 - isr0)
        loop build_idt
        lidt [idt_ptr]
        ret
