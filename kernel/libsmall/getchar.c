#include <small.h>
#include <task.h>
#include <panic.h>

extern task_t* keyboad_task;

uint32_t
getchar() {
  task_t* t = task_get();

  ASSERT_PANIC(t != NULL);

  t->waitkey = 1;
  task_block();

  return t->key;
}

void
sleep(uint32_t ms) {
  task_t* t = task_get();

  ASSERT_PANIC(t != NULL);

  t->timeout = ms;
  task_block();
}

void
wait4pid(uint32_t pid) {
  task_t* t = task_get();

  ASSERT_PANIC(t != NULL);

  t->wait4pid = pid;
  task_block();
}
