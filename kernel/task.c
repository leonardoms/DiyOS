
#include <kernel.h>

task_t* running_task, *idle_task;
static uint32_t  task_id;
uint32_t num_tasks;
uint8_t  task_enabled = 0;


__attribute__((interrupt)) void
task_schedule_handler();

void
idle() {
    // printf("task_idle ");
    while(1) {
      __asm__ __volatile__("sti");  // enable interrupts
      __asm__ __volatile__("hlt");  // idle the CPU until a interrupt fires
    }
}

void
task_enable() {
  task_enabled = 1;
}

uint8_t
task_is_enabled() {
  return (task_enabled != 0);
}

void
task_disable() {
  task_enabled = 0;
}

task_t*
task_get() {
  return running_task;
}

typedef void (*task_callback_t)(task_t* t, uint8_t* data);

void
update_wait4pid(task_t* t, uint8_t* data) {
  if(t->wait4pid == ((task_t*)data)->id) {
    t->wait4pid = 0;
    t->state = TS_READY;
  }
}

void
task_destroy() {
    printf("kill: %s (pid: %d)\n", running_task->name, running_task->id);

    task_queue_foreach(&tq_blocked, update_wait4pid, (uint32_t*)running_task);

    running_task = NULL;
    num_tasks--;
    __asm__ __volatile__("int $0xff\n");
}

task_t*
task_create(uint32_t eip, const char* name, task_state_t s) {

    task_t* t = (task_t*)malloc(sizeof(struct task));;

  	memset(t, 0, sizeof(struct task));
    t->id = ++task_id;
    t->state = s;
    t->waitkey = 0;
    memcpy((void*)name, (void*)t->name, strlen(name) + 1);
  	t->regs.eip = eip;
  	t->regs.esp = (uint32_t)malloc(4096);
  	asm volatile("mov %%cr3, %%eax":"=a"(t->regs.cr3));
  	uint32_t* stack = (uint32_t *)(t->regs.esp + 4096);
  	t->stacktop = t->regs.esp;
    *--stack = 0x10;
    *--stack = t->regs.esp;//t->stacktop + 0x1000 - 4;
  	*--stack = 0x00000202; // eflags
  	*--stack = 0x8; // cs
  	*--stack = (uint32_t)eip; // eip
  	*--stack = 0; // eax
  	*--stack = 0; // ebx
  	*--stack = 0; // ecx;
  	*--stack = 0; //edx
  	*--stack = 0; //esi
  	*--stack = 0; //edi
  	*--stack = t->regs.esp + 4096; //ebp
  	*--stack = 0x10; // ds
  	*--stack = 0x10; // fs
  	*--stack = 0x10; // es
  	*--stack = 0x10; // gs
  	t->regs.esp = (uint32_t)stack;

    t->next = NULL;

    queue_init(&t->message_queue, 32); // 32 messages buffer

    return t;
}

void
task_on_a_row(task_t* t, uint32_t* data) {
    printf("\t[%d]", t->id);
    switch (t->state) {
      case TS_RUNNING:
        printf(" [RUN] ");
        break;
      case TS_READY:
        printf(" [RDY] ");
        break;
      case TS_BLOCKED:
        printf(" [BLK] ");
        break;
    }
    printf("%s\n", t->name);
}

void
task_show_all() {

    printf("==================== TASKS  ====================\n");

    if(running_task)
      task_on_a_row(running_task, NULL);
    // printf("tq_ready\n");
    if(task_queue_size(&tq_ready) > 0)
      task_queue_foreach(&tq_ready, task_on_a_row, NULL);
    // printf("tq_blocked\n");
    if(task_queue_size(&tq_blocked) > 0)
      task_queue_foreach(&tq_blocked, task_on_a_row, NULL);

    printf("================================================\n");

}

void
task_add(task_t* t) {
    switch (t->state) {
      case TS_READY:
        task_queue_insert(&tq_ready, t);
        break;
      case TS_BLOCKED:
        task_queue_insert(&tq_blocked, t);
      default:
        break;
    }
    num_tasks++;
    // running_task->next = t;
    // t->next = running_task;
}

void
task() {
    task_id = 1000;
    num_tasks = 0;
    isr_install_callback(0xFF, task_schedule_handler); // "int 0xff" calls scheduling

    task_queue_init(&tq_ready);
    task_queue_init(&tq_blocked);

    idle_task = task_create((uint32_t)idle, "idle", TS_READY);
    task_add(idle_task);
    // running_task = idle_task;
}

void
task_start() {
  running_task = task_queue_remove(&tq_ready);
  scheduling(1);
  task_execute();
}

void
task_block() {
  ASSERT_PANIC(running_task != NULL);

  running_task->state = TS_BLOCKED;
  task_schedule_forced();

  return;
}

void
task_listen(uint32_t l) {
  ASSERT_PANIC(running_task != NULL);

  running_task->listen |= l;
}

void
task_wake(task_t* t) {
  ASSERT_PANIC(t != NULL);

  t->state = TS_READY;

  // task_schedule_forced();

  return;
}

void
_task_get_pid(task_t* t, uint32_t* data) {
  if(t->id == data[0]) {
    data[1] = (uint32_t)t;
  }
}

task_t*
task_from_pid(uint32_t pid) {
  uint32_t  t[2]; // 0: pid; 1: pointer to pid (if found)

  t[0] = pid;
  t[1] = NULL;

  task_queue_foreach(&tq_ready, _task_get_pid, t);
  task_queue_foreach(&tq_blocked, _task_get_pid, t);

  return t[1];
}

uint32_t
task_pid() {
  ASSERT_PANIC(running_task != NULL);

  return running_task->id;
}

void
task_update_queue() {
  task_t* t;
  uint32_t i;

  // update the queues
  i = task_queue_size(&tq_blocked);
  if( i > 0 ) {
    while (i--) {
      t = task_queue_remove(&tq_blocked);
      if(t->state == TS_READY) {
        task_queue_insert(&tq_ready,t);
      }
      else
        task_queue_insert(&tq_blocked,t);
    }
  }
  i = task_queue_size(&tq_ready);
  if( i > 0 ) {
    while (i--) {
      t = task_queue_remove(&tq_ready);
      if(t->state == TS_READY)
        task_queue_insert(&tq_ready,t);
      else
        task_queue_insert(&tq_blocked,t);
    }
  }
}

// execute current task last state
void
task_execute()
{
  running_task->state = TS_RUNNING;
  // asm volatile("mov %%eax, %%cr3": :"a"(running_task->regs.cr3));
  // asm volatile("xchg %bx, %bx");

	asm volatile("mov %%eax, %%esp": :"a"(running_task->regs.esp));
	asm volatile("pop %gs");
	asm volatile("pop %fs");
	asm volatile("pop %es");
	asm volatile("pop %ds");
	asm volatile("pop %ebp");
	asm volatile("pop %edi");
	asm volatile("pop %esi");
	asm volatile("pop %edx");
	asm volatile("pop %ecx");
	asm volatile("pop %ebx");
	asm volatile("pop %eax");
  // BOCHS_BREAKPOINT
	asm volatile("iret");
}

uint8_t schedule_forced = 0;

void
task_schedule_forced() {
  // if(running_task != NULL) {
    schedule_forced = 1;
    asm volatile("int $0xff");
  // }

  return;
}

// __attribute__((interrupt)) void
void
task_schedule_handler() {

  task_schedule();
}

void
task_schedule()
{
  if(schedule_forced) {
    asm volatile("add $0xc, %esp");
    schedule_forced = 0;
  } else
    asm volatile("add $0x10, %esp");


  if( running_task != NULL ) {
      // asm volatile("xchg %bx, %bx");

      asm volatile("push %eax");
      asm volatile("push %ebx");
      asm volatile("push %ecx");
      asm volatile("push %edx");
      asm volatile("push %esi");
      asm volatile("push %edi");
      asm volatile("push %ebp");
      // asm volatile("push %ds");
      // asm volatile("push %es");
      // asm volatile("push %fs");
      // asm volatile("push %gs");
      asm volatile("pushl $0x10");
      asm volatile("pushl $0x10");
      asm volatile("pushl $0x10");
      asm volatile("pushl $0x10");
      // BOCHS_BREAKPOINT
      asm volatile("mov %%esp, %%eax":"=a"(running_task->regs.esp));

      if(running_task->state == TS_RUNNING) {
          running_task->state = TS_READY;
          task_queue_insert(&tq_ready, running_task);
       } else
      if (running_task->state == TS_BLOCKED) {  // blocked task!
          task_queue_insert(&tq_blocked, running_task);
      } else
          printf("%s with invalid state (%d)\n", running_task->name, running_task->state);
    } //else task_show_all();
    task_update_queue();


    // get a ready task
    running_task = task_queue_remove(&tq_ready);
    // running_task = running_task->next;

    ASSERT_PANIC(running_task != NULL);

    running_task->state = TS_RUNNING;
        // task_show_all();
    asm volatile("mov %%eax, %%cr3": :"a"(running_task->regs.cr3));
  	asm volatile("mov %%eax, %%esp": :"a"(running_task->regs.esp));
  	asm volatile("popl %gs");
  	asm volatile("popl %fs");
  	asm volatile("popl %es");
  	asm volatile("popl %ds");
  	asm volatile("pop %ebp");
  	asm volatile("pop %edi");
  	asm volatile("pop %esi");
  	asm volatile("out %%al, %%dx": :"d"(0x20), "a"(0x20)); // send EoI to master PIC
  	asm volatile("pop %edx");
  	asm volatile("pop %ecx");
  	asm volatile("pop %ebx");
  	asm volatile("pop %eax");

  	asm volatile("iret");
}
