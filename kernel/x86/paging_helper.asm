
global page_directory_set

page_directory_set:
      mov     eax, [esp+4]    ; phys address of new page directory
;      or      eax, 3
      mov     cr3, eax
;      mov     eax, cr0
;      or      eax, 0x80000001

;      mov     cr0, eax
      ret
