extern main

section .text
    call main

    jmp   $  ; replace by sysexit
