
#include <kernel.h>
#include <list.h>

task_t* running_task = NULL;
static uint32_t  pid_n = 1000;
uint32_t num_tasks;
static list_t* tasks = NULL;


void
idle() {
    debug_printf("task_idle started.\n");
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
task_add(task_t* t) {
    tasks = list_add(tasks, t);
    num_tasks++;
}

void
tasking() {
  task_add(thread_create(idle, -1, 100, "idle"));
}

void
task_block() {
  running_task->status = TS_BLOCKED;
  // schedule();
}

void
task_listen(uint32_t l) {
  ASSERT_PANIC(running_task != NULL);

  running_task->listen |= l;
}

uint8_t
_task_get_pid(list_t* l, void* data) {

  // printf("get_pid: %x == %d ?\n", (uint32_t)data, ((task_t*)(l->data))->id);

  if( ((task_t*)(l->data))->pid == (uint32_t)data ) {
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
pid_new() {
  return ++pid_n;
}

uint32_t
pid() {
  ASSERT_PANIC(running_task != NULL);

  return running_task->pid;
}

task_t*
task_foreach(task_iterator_t it, void* udata) {
  list_t* item;

  ASSERT_PANIC( tasks != NULL );

  if( it == NULL )
    return;

  if(tasks->magic != LIST_MAGIC)
    PANIC("task list is corromped!");

  item = tasks;
  do {
    if( it( (task_t*)item->data, udata) )
      return (task_t*)item->data;
  } while( (item = item->next) != NULL );

}
