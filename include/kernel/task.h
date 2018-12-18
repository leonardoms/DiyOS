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

#pragma pack(1)
typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} task_regs_t;

typedef struct task_file {
  fs_node_t*  node;
  uint32_t    offset;
} task_file_t;

typedef struct task {
  uint8_t*   kstack;
	aspace_t*  as;
	uint32_t*  pagedir;
	uint16_t   magic;

	enum{
		TS_READY = 1, TS_BLOCKED = 2, TS_DEAD = 3
	} status;

	int32_t    priority;
	int32_t    exit_code;
	uint32_t     timeout;
	uint8_t*     kstack_mem;
	task_file_t  files[16];
	uint32_t     file_count;
	uint8_t      wait4pid;
	uint32_t   pid;
	uint32_t   ppid;
	uint8_t    name[16];
	int16_t    console;

  queue_t    message_queue;
  uint32_t   listen;

} task_t;

typedef uint8_t (*task_iterator_t)(task_t*, uint32_t*);

task_t*
task_create_from_elf(const char* name, uint8_t* runnable);

task_t*
thread_create(uint32_t init_ip, int32_t priority, uint32_t ppid, uint8_t *name);

void
task_add(task_t* t);

void
task_exit(int32_t code);

task_t*
task_get();

void
task_setup();

#define KERNEL_STACK_SIZE 0x1000

#define THREAD_MAGIC  0x1234C00F

queue_t task_died;


typedef int	spinlock_t;

static inline int xchg(volatile int *addr, int newval)
{
	int result;

	// The + in "+m" denotes a read-modify-write operand.
	asm volatile(
		"xchg %0, %1" :
		"+m" (*addr), "=a" (result) :
		"1" (newval) :
		"cc");
	return result;
}

#define SPINLOCK_ENTER(spinlock)	while(xchg(spinlock, 1) == 1) schedule();
#define SPINLOCK_LEAVE(spinlock)	xchg(spinlock, 0);

int32_t
sleep_on_without_lock(queue_t *q, uint32_t *timeout, spinlock_t *lock);

#define sleep_on(queue, timeout) sleep_on_without_lock(queue, timeout, (void*)0)

#endif
