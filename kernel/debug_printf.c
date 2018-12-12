
#include <kernel.h>

void
debug_printf (const char *format, ...);

static fs_node_t* dbg_file = NULL;
static const uint8_t* dbg_msg = { "*** Debugging started ***" };

void
debug(const uint8_t* file) {
  dbg_file = finddir_fs(fs_root_get(), file); // cannot use open/read, debug_printf is task independent.
  if( dbg_file ) {
    printf("debugging on %s\n", file);
    debug_printf("\n%s\n\n", dbg_msg);
  }
}

void
debug_putchar(uint8_t c) {
  write_fs(dbg_file, 0, 1, &c);
}

void
debug_printf (const char *format, ...) {
   uint8_t **arg = (uint8_t **) &format;
   uint8_t c;
   uint8_t buf[20];

   arg++;

   while((c = *format++) != 0) {
       if (c != '%')
         debug_putchar (c);
       else {
         uint8_t *p, *p2;
         int pad0 = 0, pad = 0;

         c = *format++;
         if (c == '0') {
             pad0 = 1;
             c = *format++;
         }

         if (c >= '0' && c <= '9') {
             pad = c - '0';
             c = *format++;
         }

         switch (c) {
           case 'X':
           case 'd':
           case 'u':
           case 'x':
             itoa (buf, c, *((int *) arg++));
             p = buf;
             goto string;
             break;
           case 's':
             p = *arg++;
             if (! p)
               p = (uint8_t*)"(null)";
string:
             for (p2 = p; *p2; p2++);
             for (; p2 < p + pad; p2++)
               debug_putchar (pad0 ? '0' : ' ');
             while (*p)
               debug_putchar (*p++);
             break;
           default:
             debug_putchar (*((int *) arg++));
             break;
           }
         }
     }
}
