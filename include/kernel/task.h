#ifndef __TASK_H__
#define __TASK_H__

#include <kernel.h>
#include <queue.h>

#define LISTEN_ALL  0xFFFFFFFF
#define LISTEN_NONE 0
#define IDLE        (1 << 0)
#define TIMER       (1 << 1)
#define KEYBOARD    (1 << 2)
#define MOUSE       (1 << 3)

typedef enum { TS_RUNNING = 0, TS_READY = 1, TS_BLOCKED = 3 } task_state_t;

typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} task_regs_t;

typedef struct task {
    task_regs_t   regs;
    uint32_t      stacktop;
    uint32_t      id;
    uint32_t      listen;
    task_state_t  state;
    int32_t     timeout; // timeout in seconds, used in sleep
    uint32_t    wait4pid;
    uint8_t     waitkey, key;
    char        name[1024];

    queue_t     message_queue;

    fs_node_t*   files[16];
    uint32_t     file;

    struct task *next;
} task_t;

typedef void (*task_iterator_t)(task_t*, uint32_t*);

task_t*
task_create_from_elf(const char* name, uint8_t* runnable);

task_t*
task_create(uint32_t eip, const char* name, task_state_t s);

void
task_add(task_t* t);

void
task_destroy();

void
task_show_all();

task_t*
task_get();

void
task_setup();

void
task_schedule();

extern void
task_schedule_forced();

#endif
