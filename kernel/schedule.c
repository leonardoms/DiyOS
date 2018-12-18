
#include <kernel.h>

extern void switch_to(task_t *task);

extern task_t* running_task;

uint8_t
_find_higher_priority(task_t* t, uint32_t* udata) {

  if( t->status == TS_READY )
  if( t->priority > *(int32_t*)udata )
    *(int32_t*)udata = t->priority;

  return 0;
}

uint8_t
_higher_task(task_t* t, uint32_t* udata) {

  if( t->status == TS_READY )
  if( t->priority == *(int32_t*)udata )
    return 1;

  return 0;
}

void schedule() {

	static uint32_t current = 0;
	int32_t  i, p = -127;
	task_t*  t;
  //
  // if(running_task)
  //   debug_printf("[SCHED] leave %s (%d),", running_task->name, running_task->pid);

  task_foreach(_find_higher_priority,&p);

  t = task_foreach(_higher_task,&p);

  // debug_printf(" resume %s (%d).\n", t->name, t->pid);

	switch_to(t);

}
