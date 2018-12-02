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

    fs_node_t   files[16];

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
