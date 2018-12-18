#include <kernel.h>

void
sleep(uint32_t ms) {
  task_t* t = task_get();

  ASSERT_PANIC(t != NULL);

  t->timeout = ms;
  t->status = TS_BLOCKED;

  schedule();
}
