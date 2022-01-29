#include <kernel.h>
#include <ring.h>

console_t*
console_get() {
  task_t* t;

  if( (t = task_get()) == NULL ) {
    PANIC("no console.");
  }

  return t->console;
}

console_t*
console_create() {
  // task_create open two files pointed to /dev/tty that is STDIN/STDOUT and STDERR
  console_t* con;

  con = (console_t*)malloc(sizeof(console_t));
  if( con == NULL )
    return NULL;

  ring_init(&con->out, 2* (80 * 25 * 2)); // 80x25 text mode x2
  ring_init(&con->in, 64);

  // int i = 0;
  //
  // while( ring_is_full(&con->out) == FALSE ) {
  //   ring_push(&con->out, '0' + (uint8_t)i++);
  //   if( i > 32 ) i = 0;
  // }
  //
  // uint8_t c;
  // while(!ring_is_empty(&con->out)) {
  //   ring_pop(&con->out, &c);
  //   printf("%c",c);
  // }

  return con;
}
