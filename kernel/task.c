
#include <task.h>
#include <panic.h>

task_t* running_task, *idle_task;
static uint32_t  task_id;
uint32_t num_tasks;

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

task_t*
task_get() {
  return running_task;
}

typedef void (*task_callback_t)(task_t* t, uint8_t* data);

void
task_foreach(task_callback_t c, uint8_t* data) {
  task_t* t;

  t = running_task;
  do {
    c(t, data);
    t = t->next;
  } while( t != running_task );
}

void
task_destroy() {
    printf("[RIP] %s task \n", running_task->name, running_task->id);
    running_task = NULL;
    num_tasks--;
    // task_show_all();
    __asm__ __volatile__("int $0xff\n");
}

task_t*
task_create(uint32_t eip, const char* name, task_state_t s) {

    uint32_t* stack_esp;

    task_t* t = (task_t*)kmalloc(sizeof(struct task));;

  	memset(t, 0, sizeof(struct task));
    t->id = ++task_id;
    t->time_ms = 0;
    t->state = s;
    t->waitkey = 0;
    memcpy((void*)name, (void*)t->name, strlen(name) + 1);
  	t->regs.eip = eip;
  	t->regs.esp = (uint32_t)kmalloc(4096);
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
  	*--stack = t->regs.esp; //ebp
  	*--stack = 0x10; // ds
  	*--stack = 0x10; // fs
  	*--stack = 0x10; // es
  	*--stack = 0x10; // gs
  	t->regs.esp = (uint32_t)stack;
    *stack_esp = t->stacktop;

    t->next = NULL;

    return t;
}

task_t*
task_create_from_elf(const char* name, uint8_t* runnable) {

    task_t*   t;
    sect_t*   s;
    uint32_t err, i, entry, *pd;
    t = (task_t*)kmalloc(sizeof(struct task));

    ASSERT_PANIC(name);
    ASSERT_PANIC(runnable);

    printf("[M] %s ... ", name);

    if(elf32_check(runnable,ET_EXEC) == FALSE) {
        printf("invalid elf executable.\n");
        return NULL;
    }

    t->aspace = aspace_create();
    if(t->aspace == NULL) {
        // kfree(t);
        return NULL;
    }

    err = load_elf_exec((char*)runnable, (unsigned int*)&entry, t->aspace);
    if(err != 0) { // has error code?
        printf("failed to load elf executable. (err: %d)\n", name, err);
        // aspace_destroy(t->aspace);
        // kfree(t);
        return NULL;
    }

    // alloc task to 0x800000 and stack to 0xBF800000
    //TODO: change to kmalloc_phys getting virtual and phys addr
    t->pagedir_phys = (uint32_t)kmalloc_p(0x1000);
    t->task_phys = (uint32_t)kmalloc_p(0x4000);//
    t->stack_phys = (uint32_t)kmalloc_p(0x2000);

    __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(pd)::"%eax");
    pd = (uint32_t*)( (uint32_t)pd & 0xFFFFF000 );

    // copy the Kernel Page tables
    for( i = 768; i < 1024; i++ ) {
      ((uint32_t*)t->pagedir_phys)[i] = pd[i] | PAGE_FLAG_RW | PAGE_FLAG_PRESENT; // force all tables present.
    }
    ((uint32_t*)t->pagedir_phys)[0] = pd[0]; // first 8MB is kernel mapped area 1:1 !
    ((uint32_t*)t->pagedir_phys)[1] = pd[1];
    // ((uint32_t*)t->pagedir_phys)[2] = pd[2];
    // ((uint32_t*)t->pagedir_phys)[3] = pd[3];

    // maps the task space on task page directory
    ((uint32_t*)t->pagedir_phys)[USER_ADDRESS_BASE >> 22] = t->task_phys | PAGE_FLAG_RW | PAGE_FLAG_PRESENT | PAGE_FLAG_USER;
    ((uint32_t*)t->pagedir_phys)[USER_STACK_ADDRESS_BASE >> 22] = t->stack_phys | PAGE_FLAG_RW | PAGE_FLAG_PRESENT | PAGE_FLAG_USER;

    ((uint32_t*)t->task_phys)[0] = (uint32_t)kmalloc_p(0x1000) | PAGE_FLAG_RW | PAGE_FLAG_PRESENT | PAGE_FLAG_USER;
    ((uint32_t*)t->task_phys)[1] = (uint32_t)kmalloc_p(0x1000) | PAGE_FLAG_RW | PAGE_FLAG_PRESENT | PAGE_FLAG_USER;
    ((uint32_t*)t->task_phys)[2] = (uint32_t)kmalloc_p(0x1000) | PAGE_FLAG_RW | PAGE_FLAG_PRESENT | PAGE_FLAG_USER;
    ((uint32_t*)t->task_phys)[3] = (uint32_t)kmalloc_p(0x1000) | PAGE_FLAG_RW | PAGE_FLAG_PRESENT | PAGE_FLAG_USER;
    ((uint32_t*)t->stack_phys)[0] = (uint32_t)kmalloc_p(0x1000) | PAGE_FLAG_RW | PAGE_FLAG_PRESENT | PAGE_FLAG_USER;
    ((uint32_t*)t->stack_phys)[1] = (uint32_t)kmalloc_p(0x1000) | PAGE_FLAG_RW | PAGE_FLAG_PRESENT | PAGE_FLAG_USER;

    for(i = 0; i < t->aspace->sects; i++) {
        s = &t->aspace->sect[i];

        if( (s->addr < USER_ADDRESS_BASE) || (s->addr >= KERNEL_ADDRESS_BASE) ) {
            printf("section out of user memory space (0x%x). \n", name, s->addr);
            // aspace_destroy(t->aspace);
            // kfree(t);
            return NULL;
        }

        memcpy((void*)(runnable + s->offset), (void*)(((uint32_t*)t->task_phys)[0] & 0xFFFFF000), s->size);

    }

    t->id = ++task_id;
    t->time_ms = 0;
    t->state = TS_READY;
    t->waitkey = 0;
    memcpy((void*)name, (void*)t->name, strlen((uint8_t*)name) + 1);
    t->regs.eax = 0;
    t->regs.ebx = 0;
    t->regs.ecx = 0;
    t->regs.edx = 0;
    t->regs.esi = 0;
    t->regs.edi = 0;
    t->regs.eip = (uint32_t)entry;
    t->regs.ebp = USER_STACK_ADDRESS_BASE + USER_STACK_SIZE - sizeof(int);
    t->regs.esp = USER_STACK_ADDRESS_BASE + USER_STACK_SIZE - sizeof(int);
    // __asm__ __volatile__("pushf; popl %%eax; mov %%eax, %0":"=m"(t->regs.eflags)::"%eax");
    t->regs.eflags = 0x200; // USER level task (to allow syscall interrupt)
    t->regs.cr3 = t->pagedir_phys;

    t->next = NULL;
    t->switch_function = context_switch;

    // printf("ok (id=%d entry=0x%x cr3=0x%x)\n", t->id, t->regs.eip, t->regs.cr3);
    printf("loaded.\n");
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
task_setup() {

    task_id = 0;
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
  // task_show_all();
  task_execute();
  // task_schedule_forced();

  PANIC("task_start() fails.")
}

void
task_block() {
  ASSERT_PANIC(running_task != NULL);

  running_task->state = TS_BLOCKED;
  task_schedule_forced();

  return;
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
        // t->switch_function(t->regs);
        // task_show_all();
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
  asm volatile("mov %%eax, %%cr3": :"a"(running_task->regs.cr3));
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

void
task_schedule_forced() {
  // if(running_task != NULL) {
    asm volatile("int $0xff");
  // }

  return;
}

__attribute__((interrupt)) void
task_schedule_handler() {
  // asm volatile("add $0x1c, %esp");
  task_schedule();
}

void
task_schedule()
{

  if( running_task != NULL ) {
      asm volatile("add $0x1c, %esp");
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
    // BOCHS_BREAKPOINT
  	asm volatile("iret");
}
