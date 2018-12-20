

global switch_to
extern running_task

switch_to:

    pop     eax		; convert CALL stack frame (EIP only)...
    pushf		      ; ...to partial IRET (EIP, EFLAGS)
    push    eax

    push    ebp	  ; callee-save registers used by C
    push    edi	  ; pushing these creates kregs_t stack frame
    push    esi
    push    ebx
    cli
    mov     eax, [running_task]
    xchg bx, bx

    ; store current kernel ESP in thread_t struct of current thread/task
    mov     [eax], esp
    xchg bx, bx

    ; get pointer (thread_t t) to new task/thread to run
    mov     eax,  [esp + 24]

    mov     ebx, [eax + 8]
    cmp     ebx, dword 0
    je      no_cr3

    ; Carregando o novo pagedir
    mov     cr3, ebx

no_cr3:

    ; make this the current thread
    mov     [running_task], eax
    ; load new ESP
    mov     esp,[eax]
    pop     ebx	; pop C registers
    pop     esi
    pop     edi
    pop     ebp

    pop     eax	; convert partial IRET stack frame (EIP, EFLAGS)...
    push    cs	; ...to full IRET stack frame (EIP, CS, EFLAGS)
    push    eax

    iret		; IRET to new kernel thread
