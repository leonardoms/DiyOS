#ifndef __TASK_H__
#define __TASK_H__

#include <ctypes.h>

extern void initTasking();

typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} task_regs_t;

typedef struct task {
    task_regs_t regs;
    uint32_t    pagedir[1024];
    struct task *next;
} task_t;

typedef struct sect {
    uint32_t    addr, size, flags, offset;
} sect_t;

extern void initTasking();
extern void createTask(tast_t*, void(*)(), uint32_t, uint32_t*);

extern void yield(); // Switch task frontend
extern void switchTask(Registers *old, Registers *new); // The function which actually switches

#endif
