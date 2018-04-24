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

extern irq_dispacher
extern isr_dispacher

isr_handler:
    push    gs
    push    fs
    push    es
    push    ds
    pusha

    mov     ax, 0x10         ; kernel data segment
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

    mov     ax, 0x10         ; kernel data segment
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
  	push    byte 0		       ; error code (fake)
    push    byte 0           ; isr number
	  jmp     near isr_handler

isr1:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 1           ; isr number
	  jmp     near isr_handler

isr3:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 3           ; isr number
	  jmp     near isr_handler

isr4:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 4           ; isr number
	  jmp     near isr_handler

isr5:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 5           ; isr number
	  jmp     near isr_handler

isr6:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 6           ; isr number
	  jmp     near isr_handler

isr7:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 7           ; isr number
	  jmp     near isr_handler

isr8:
    cli
                             ; error code already pushed
    push    byte 8           ; isr number
	  jmp     near isr_handler

isr9:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 9           ; isr number
	  jmp     near isr_handler

isr10:
    cli
                              ; error code already pushed
    push    byte 10           ; isr number
	  jmp     near isr_handler

isr11:
    cli
                              ; error code already pushed
    push    byte 11           ; isr number
	  jmp     near isr_handler

isr12:
    cli
                              ; error code already pushed
    push    byte 12           ; isr number
	  jmp     near isr_handler

isr13:
    cli
                              ; error code already pushed
    push    byte 13           ; isr number
	  jmp     near isr_handler

isr14:
    cli
                              ; error code already pushed
    push    byte 14           ; isr number
	  jmp     near isr_handler

isr15:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 15          ; isr number
	  jmp     near isr_handler

isr16:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 16          ; isr number
	  jmp     near isr_handler

isr17:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 17          ; isr number
	  jmp     near isr_handler

isr18:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 18          ; isr number
	  jmp     near isr_handler

isr19:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 19          ; isr number
	  jmp     near isr_handler

isr20:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 20          ; isr number
	  jmp     near isr_handler

isr21:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 21          ; isr number
	  jmp     near isr_handler

isr22:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 22          ; isr number
	  jmp     near isr_handler

isr23:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 23          ; isr number
	  jmp     near isr_handler

isr24:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 24          ; isr number
	  jmp     near isr_handler

isr25:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 25          ; isr number
	  jmp     near isr_handler

isr26:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 26          ; isr number
	  jmp     near isr_handler

isr27:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 27          ; isr number
	  jmp     near isr_handler

isr28:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 28          ; isr number
	  jmp     near isr_handler

isr29:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 29          ; isr number
	  jmp     near isr_handler

isr30:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 30          ; isr number
	  jmp     near isr_handler

isr31:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 31          ; isr number
	  jmp     near isr_handler

;===============================================================================
; IRQ 0 ~ 15 maped to 32 ~ 47
;===============================================================================

isr32:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 32          ; isr number
	  jmp     near irq_handler

isr33:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 33          ; isr number
	  jmp     near irq_handler

isr34:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 34          ; isr number
	  jmp     near irq_handler

isr35:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 35          ; isr number
	  jmp     near irq_handler

isr36:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 36          ; isr number
	  jmp     near irq_handler

isr37:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 37          ; isr number
	  jmp     near irq_handler

isr38:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 38          ; isr number
	  jmp     near irq_handler

isr39:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 39          ; isr number
	  jmp     near irq_handler

isr40:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 40          ; isr number
	  jmp     near irq_handler

isr41:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 41          ; isr number
	  jmp     near irq_handler

isr42:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 42          ; isr number
	  jmp     near irq_handler

isr43:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 43          ; isr number
	  jmp     near irq_handler

isr44:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 44          ; isr number
	  jmp     near irq_handler

isr45:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 45          ; isr number
	  jmp     near irq_handler

isr46:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 46          ; isr number
	  jmp     near irq_handler

isr47:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 47          ; isr number
	  jmp     near irq_handler

;===============================================================================
; Syscall at int 0x42
;===============================================================================

isr66:
    cli
  	push    byte 0		       ; error code (fake)
    push    byte 66          ; isr number
	  jmp     near isr_handler
