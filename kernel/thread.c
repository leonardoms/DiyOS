
#include <kernel.h>
#include <queue.h>
#include <ctypes.h>

task_t*
thread_create(uint32_t init_ip, int32_t priority, uint32_t ppid, uint8_t *name) {
	kregs_t *kregs;
	task_t *t;
	uint32_t i;

	t = (task_t*)malloc(sizeof(task_t));

	t->kstack_mem = (uint32_t*)malloc(KERNEL_STACK_SIZE);
	if(t->kstack_mem == NULL) {
		free(t);
		return NULL;
	}

	strncpy(t->name, name, 16);
	t->wait4pid = false;
	t->ppid = ppid;
	t->pid = pid_new();
	t->status = TS_READY;
	t->priority = priority;
	t->magic = THREAD_MAGIC;

	__asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(t->pagedir)::"%eax");


	/* Coloca o primeiro contexto(kregs) na pilha para ser restaurando pelo primeiro switch_to(). */
	t->kstack = t->kstack_mem + KERNEL_STACK_SIZE - sizeof(kregs_t);
	kregs = (kregs_t *)t->kstack;

	kregs->eip 	= init_ip;
	kregs->eflags = 0x200;

	queue_init(&t->message_queue,32);

	t->file_count = 0;

	return t;
}

int32_t
sleep_on_without_lock(queue_t *q, uint32_t *timeout, spinlock_t *lock) {
	task_t*		t;

	t = task_get();

	t->status = TS_BLOCKED;

	queue_add(q, t);

	if( timeout != NULL ) {
		t->timeout = *timeout;
	}	else {
		t->timeout = 0;
	}

	if( lock != NULL )
		SPINLOCK_LEAVE(lock);

	schedule();

	if( timeout != NULL ) {
		*timeout = t->timeout;
		if( *timeout == 0 ) {
			return -1;
		}
	}


	return 0;
}

void
wake_up(queue_t *q) {
	task_t *t;

	t = queue_remove(q);

	if(t == NULL)
		return;

	t->status = TS_READY;
}

void
task_wake_after_dead(task_t* t, uint32_t* data) {
  task_t* cur = task_get(); // get the dead task

  if( cur->status != TS_DEAD ) {
    debug_printf("\'%s\' say: i'm not dead!");
    return;
  }

  if( t->pid == cur->ppid ) {
    if( t->wait4pid ) {
      t->wait4pid = false;
      t->status = TS_READY;
    }
  }

}

void
thread_destroy(task_t *t) {

	if( t->magic != THREAD_MAGIC ) {
		debug_printf("thread_destroy(): argument is not a thread!\n");
    return;
	}

  list_remove(t);

  free(t->kstack_mem);
  free(t);

}

void task_exit(int32_t code){
	int i;

  task_t* t = task_get();

	t->exit_code = code;
	t->status = TS_DEAD;

	// wake_up(&task_died); // ???????????

  task_foreach(task_wake_after_dead, NULL);

	schedule();
}
