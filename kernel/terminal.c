
#include <small.h>
#include <task.h>
#include <drivers/fb.h>

#define BUFFER_SIZE 1024

uint8_t buffer[BUFFER_SIZE];
uint32_t buffer_idx;

char curr_dir[] = "/initrd/";

char
exec(char* cmd);

char*
getpwd() {
  return curr_dir;
}

void
buffer_clear() {
  buffer_idx = 0;
  buffer[buffer_idx] = '\0';
}

void
buffer_delete() {
  if(buffer_idx > 0) {
    buffer_idx--;
    buffer[buffer_idx] = '\0';
  }
}

void
buffer_add(char c) {
  if(buffer_idx > BUFFER_SIZE) {
    printf("terminal_root: buffer is full.\n");
    while(1);
    return;
  }

  buffer[buffer_idx++] = c;
  buffer[buffer_idx] = 0;
}

void
terminal_root() {
  static uint32_t c;
  int pid = 0;

  buffer_clear();
  fb_clear();
  fb_color(FC_DGRAY | BC_WHITE);
  printf("_______________________________Welcome to ");
  fb_color(FC_BLACK | BC_WHITE);
  printf("DiyOS");
  fb_color(FC_DGRAY | BC_WHITE);
  printf("_________________________________\n");
  fb_color(FC_LGRAY | BC_BLACK);

  printf("%s@%s~# ", "root", "/initrd/");

  while(1) {
    // BOCHS_BREAKPOINT;
    c = getchar();
    // BOCHS_BREAKPOINT;

    printf("%c", (char)c);
    // sleep(1000);
    switch (c) {
      case '\n':
          if(buffer_idx > 0) {
            if(!exec(buffer))
              printf("%s: invalid command!\n", buffer);
          }

          printf("%s@%s~# ", "root", "/initrd/");
          buffer_clear();
          break;
      case '\b':
          buffer_delete();
          break;
      default:
          buffer_add(c);
          break;
    }

  } // while(1)

}

char
exec(char* cmd) {
  if(!strcmp(cmd,"clear"))
    fb_clear();
    else if(!strcmp(cmd,"task"))
      task_show_all();
      else if(!strcmp(cmd,"reboot")) {
        acpi_power_down();
        acpi_reset();
      }
      else return 0;

  return 1;
}

void
terminal_bar() {

  uint32_t old_x, old_y, color, time_elapsed = 0, h, m, s, x;

  while(1) {
      fb_getxy(&old_x, &old_y); // save cursor position
      fb_get_color(&color); // save color

      h = time_elapsed / 3600;
      m = (time_elapsed % 3600) / 60;
      s = time_elapsed % 60;

      fb_gotoxy(0,0);
      fb_color(FC_DGRAY | BC_WHITE);
      printf(" Welcome to ");
      fb_color(FC_BLACK | BC_WHITE);
      printf("DiyOS ");
      fb_color(FC_DGRAY | BC_WHITE);
      printf("| uptime %02d:%02d:%02d", h, m, s );

      while(1) {
        fb_getxy(&x,NULL);
        if(x >= (80 - 1))
          break;
        printf(" ");  // complete the line

      }

      fb_color(color);
      fb_gotoxy(old_x, old_y);

      sleep(1000);
      time_elapsed++;
  }
}

void
terminal_root_setup() {
  task_t* t;

  t = task_create(terminal_root, "terminal_root", TS_READY);
  task_add(t);

  t = task_create(terminal_bar, "terminal_bar", TS_READY);
  task_add(t);

}
