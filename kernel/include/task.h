#ifndef __TASK_H__
#define __TASK_H__

#include <ctypes.h>
#include <elf.h>
#include <aspace.h>
#include <x86/x86.h>
#include <x86/memory.h>

typedef enum { TS_RUNNING = 0, TS_READY = 1, TS_BLOCKED = 3 } task_state_t;

typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} task_regs_t;

typedef struct task {
    task_regs_t   regs;
    uint32_t      stacktop;
    uint32_t      id;
    task_state_t  state;
    uint32_t    time_ms;
    int32_t     timeout; // timeout in seconds, used in sleep
    uint8_t     waitkey, key;
    int32_t     quatum; // ticks of time left for running
    char        name[1024];
    aspace_t*   aspace;
    uint32_t    pagedir_phys; // page directory physical address
    uint32_t    task_phys;
    uint32_t    stack_phys; //pagedir[1024],
    uint32_t    *pagedir_virt;  // page directory virt address, used for kfree
    uint32_t    *task_virt; // used for kfree
    uint32_t    *stack_virt; // used for kfree
    void        (*switch_function)(task_regs_t regs);
    struct task *next;
} task_t;

#define TASK_MAX 32

typedef struct task_queue {
  task_t* task[TASK_MAX];
  int32_t front, rear;
  uint32_t  count;
} task_queue_t;

typedef void (*task_queue_callback_t)(task_t*, uint32_t*);

void
task_queue_init(task_queue_t* q);

// get first task in queue
task_t*
task_queue_peek(task_queue_t* q);

// remove first task in queue and returns that
task_t*
task_queue_remove(task_queue_t* q);

// get queue size
uint32_t
task_queue_size(task_queue_t* q);

void
task_queue_insert(task_queue_t* q, task_t* t);

// run custom callback on each entry in queue
void
task_queue_foreach(task_queue_t* q, task_queue_callback_t c, uint32_t* data);

task_queue_t   tq_blocked, tq_ready;

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
