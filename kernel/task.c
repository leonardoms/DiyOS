
#include <kernel.h>
#include <list.h>

task_t* running_task, *idle_task;
static uint32_t  task_id;
uint32_t num_tasks;
static list_t* tasks = NULL, *tasks_cur;
static queue_t wake_queue; //test.. priorize task waiting for wakeup

task_t*
task_next();

void
idle() {
    // printf("task_idle ");
    while(1) {
      // __asm__ __volatile__("sti");  // enable interrupts
      __asm__ __volatile__("hlt");  // idle the CPU until a interrupt fires
    }
}


task_t*
task_get() {
  return running_task;
}

void
task_destroy() {
    printf("kill: %s (pid: %d)\n", running_task->name, running_task->id);

    // list_foreach(tasks, update_wait4pid, (uint32_t*)running_task);

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
    t->file = 0;
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
task_add(task_t* t) {
    tasks = list_add(tasks, t);
    num_tasks++;
}

void
task() {
    task_id = 1000;
    num_tasks = 0;
    isr_install_callback(0xFF, task_schedule); // "int 0xff" calls scheduling

    tasks = NULL;
    queue_init(&wake_queue, 16);

    idle_task = task_create((uint32_t)idle, "idle", TS_READY);
    task_add(idle_task);
}

void
task_start() {
  tasks_cur = tasks;

  running_task = task_next();

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
  queue_add(&wake_queue, t);

  return;
}

uint8_t
_task_get_pid(list_t* l, void* data) {

  // printf("get_pid: %x == %d ?\n", (uint32_t)data, ((task_t*)(l->data))->id);

  if( ((task_t*)(l->data))->id == (uint32_t)data ) {
    return FALSE;
  }
  return TRUE;
}

static uint32_t  t[2]; // 0: pid; 1: pointer to pid (if found)

task_t*
task_from_pid(uint32_t pid) {
  list_t  *l;

  l = list_foreach(tasks, _task_get_pid, (uint32_t*)pid);

  if( l )
    return (task_t*)l->data;

  return NULL;
}

uint32_t
task_pid() {
  ASSERT_PANIC(running_task != NULL);

  return running_task->id;
}

// execute current task last state
void
task_execute()
{
  ASSERT_PANIC( running_task != NULL );

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

task_t*
task_next() {
  list_t* cur;
  task_t* wake_me;

  ASSERT_PANIC( tasks != NULL );
  ASSERT_PANIC( tasks_cur != NULL );

  if( (wake_me = queue_remove(&wake_queue)) != NULL ) // has task waiting for wake up?
    return wake_me;

  if( tasks_cur->next == NULL )
    cur = tasks; // points to start of list
  else
    cur = tasks_cur->next;

  ASSERT_PANIC( cur != NULL );

  while( ((task_t*)cur->data)->state != TS_READY ) {
    cur = cur->next;
    if( cur == NULL ) {
      // PANIC( "No tasks to run. (where's Idle?)" );
      // break;
      cur = tasks;
    }
  }

  ASSERT_PANIC( cur != NULL );

  tasks_cur = cur;

  return (task_t*)cur->data;
}

void
task_foreach(task_iterator_t it, void* udata) {
  list_t* item;

  ASSERT_PANIC( tasks != NULL );

  if( it == NULL )
    return;

  if(tasks->magic != LIST_MAGIC)
    PANIC("task list is corromped!");

  item = tasks;
  do {
    it( (task_t*)item->data, udata);
  } while( (item = item->next) != NULL );

}

void
task_schedule()
{
  if(schedule_forced) {
    asm volatile("add $0xc, %esp");
    schedule_forced = 0;
  } else
    asm volatile("add $0x10, %esp");

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
    asm volatile("mov %%esp, %%eax":"=a"(running_task->regs.esp));

    if(running_task->state == TS_RUNNING) {
        running_task->state = TS_READY;
     }


    running_task = task_next();

    ASSERT_PANIC(running_task != NULL);

    running_task->state = TS_RUNNING;
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
