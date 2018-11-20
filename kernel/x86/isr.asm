global isr0
global isr1
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31
global isr32
global isr33
global isr34
global isr35
global isr36
global isr37
global isr38
global isr39
global isr40
global isr41
global isr42
global isr43
global isr44
global isr45
global isr46
global isr47
global isr66
global isr255

extern irq_dispacher
extern isr_dispacher
extern timer_handle
extern keyboard_handle
extern task_schedule


isr_handler:
    ; CPU pushed for me:
    ;       eip
    ;       cs
    ;       eflags
    ;       useresp
    ;       ss
    ; ...and I pushed:
    ;       error_code
    ;       isr number

    push    gs
    push    fs
    push    es
    push    ds
    pusha

    mov     eax, 0x10         ; kernel data segment
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    call    isr_dispacher

    popa
    pop     ds
    pop     es
    pop     fs
    pop     gs

    add     esp, 8    ; error code / isr number
    iret

irq_handler:
    push    gs
    push    fs
    push    es
    push    ds
    pusha

    mov     eax, 0x10         ; kernel data segment
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    call    irq_dispacher

    popa
    pop     ds
    pop     es
    pop     fs
    pop     gs

    add     esp, 8    ; error code / isr number
    iret

isr0:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 0           ; isr number
	  jmp     near isr_handler

isr1:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 1           ; isr number
	  jmp     near isr_handler

isr3:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 3           ; isr number
	  jmp     near isr_handler

isr4:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 4           ; isr number
	  jmp     near isr_handler

isr5:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 5           ; isr number
	  jmp     near isr_handler

isr6:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 6           ; isr number
	  jmp     near isr_handler

isr7:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 7           ; isr number
	  jmp     near isr_handler

isr8:
    cli
                             ; error code already pushed
    push    dword 8           ; isr number
	  jmp     near isr_handler

isr9:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 9           ; isr number
	  jmp     near isr_handler

isr10:
    cli
                              ; error code already pushed
    push    dword 10           ; isr number
	  jmp     near isr_handler

isr11:
    cli
                              ; error code already pushed
    push    dword 11           ; isr number
	  jmp     near isr_handler

isr12:
    cli
                              ; error code already pushed
    push    dword 12           ; isr number
	  jmp     near isr_handler

isr13:
    cli
                              ; error code already pushed
    push    dword 13           ; isr number
	  jmp     near isr_handler

isr14:
    cli
                              ; error code already pushed
    push    dword 14           ; isr number
	  jmp     near isr_handler

isr15:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 15          ; isr number
	  jmp     near isr_handler

isr16:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 16          ; isr number
	  jmp     near isr_handler

isr17:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 17          ; isr number
	  jmp     near isr_handler

isr18:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 18          ; isr number
	  jmp     near isr_handler

isr19:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 19          ; isr number
	  jmp     near isr_handler

isr20:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 20          ; isr number
	  jmp     near isr_handler

isr21:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 21          ; isr number
	  jmp     near isr_handler

isr22:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 22          ; isr number
	  jmp     near isr_handler

isr23:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 23          ; isr number
	  jmp     near isr_handler

isr24:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 24          ; isr number
	  jmp     near isr_handler

isr25:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 25          ; isr number
	  jmp     near isr_handler

isr26:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 26          ; isr number
	  jmp     near isr_handler

isr27:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 27          ; isr number
	  jmp     near isr_handler

isr28:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 28          ; isr number
	  jmp     near isr_handler

isr29:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 29          ; isr number
	  jmp     near isr_handler

isr30:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 30          ; isr number
	  jmp     near isr_handler

isr31:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 31          ; isr number
	  jmp     near isr_handler

;===============================================================================
; IRQ 0 ~ 15 maped to 32 ~ 47
;===============================================================================



isr34:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 34          ; isr number
	  jmp     near irq_handler

isr35:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 35          ; isr number
	  jmp     near irq_handler

isr36:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 36          ; isr number
	  jmp     near irq_handler

isr37:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 37          ; isr number
	  jmp     near irq_handler

isr38:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 38          ; isr number
	  jmp     near irq_handler

isr39:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 39          ; isr number
	  jmp     near irq_handler

isr40:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 40          ; isr number
	  jmp     near irq_handler

isr41:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 41          ; isr number
	  jmp     near irq_handler

isr42:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 42          ; isr number
	  jmp     near irq_handler

isr43:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 43          ; isr number
	  jmp     near irq_handler

isr44:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 44          ; isr number
	  jmp     near irq_handler

isr45:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 45          ; isr number
	  jmp     near irq_handler

isr46:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 46          ; isr number
	  jmp     near irq_handler

isr47:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 47          ; isr number
	  jmp     near irq_handler

;===============================================================================
; Syscall at int 0x42
;===============================================================================
isr66:
    cli
  	push    dword 0		       ; error code (fake)
    push    dword 66          ; isr number
	  jmp     near isr_handler

;===============================================================================
; Keyboard
;===============================================================================
isr33:
    cli
    xchg bx, bx
    call keyboard_handle

;===============================================================================
; PIT
;===============================================================================
isr32:
    cli
	  call timer_handle


;===============================================================================
; TaskSchedule at int 0xFF
;===============================================================================
isr255:
    cli
	  call task_schedule
