// keyboard built-in
#include <drivers/kb.h>
#include <kernel.h>

task_t* kb_task;
static queue_t   kb_queue;


void
keyboard_listen(task_t* t, uint32_t* data) {
  if(t->listen && KEYBOARD) {
    // printf("message_to %d: '0x%x'\n", t->id, (uint16_t)data);
    message_to(t->id, data, 0); // send a fake pointer with value of the key code!
  }
}

void
keyboad_task() {

  static uint8_t code, y = 0, scode;
  static uint32_t  packet;

  while(1) {

      scheduling(0);

      while( ( scode = (uint8_t)queue_remove(&kb_queue) ) != NULL ) {

        code = (uint8_t)convert(scode);
        if(code) {

          if(scode & KEY_RELEASED)
            packet = 0;
          else
            packet = 1;

          packet <<= 8;
          packet |= code;
          // printf("%x packet\n", packet);

          task_foreach(keyboard_listen, (uint32_t*)(packet) );
        }
      }
      scheduling(1);

      task_block(); // block 'keyboard' task
  }
}

void keyboard_handler() {
    asm volatile("cli");
    asm volatile("add $0xc, %esp");
    asm volatile("pusha");

    queue_add(&kb_queue, (uint32_t*)inportb(0x60)); //TODO: add on kb_queue of the active console
    pic_acknowledge(1);
    task_wake(kb_task);

    asm volatile("popa");
    asm volatile("iret");
}

void kb() {

    kb_task = task_create(keyboad_task, "keyboard", TS_BLOCKED);
    ASSERT_PANIC(kb_task != NULL);

    queue_init(&kb_queue, 16);

    kb_task->id = KEYBOARD; // force keyboard pid
    task_add(kb_task);

    irq_enable(1);
}

// adapted from Projeto-SOmBRA kbd.c
int32_t convert(uint32_t code){
	static uint8_t k_shift = 0;
	static uint8_t k_alt   = 0;
	static uint8_t k_ctrl  = 0;

	static uint8_t k_num    = 0;
	static uint8_t k_caps   = 0;
	static uint8_t k_scroll = 0;

	uint32_t  key   = 0;
	uint8_t leds = false;
	uint8_t  i;

/* Verifica se o código da tecla é menor ou igual ao tamanho do keymap. */
	if (code>=KEYMAP_SIZE){
		return 0;
	}

	if (code & KEY_RELEASED){
		code &= 0x7F;
		switch(code){
			case LSHIFT:
			case RSHIFT:
				k_shift = 0;
			return 0;
			case ALT:
				k_alt   = 0;
			return 0;
			case CTRL:
				k_ctrl  = 0;
			default:
				break; // modified
		}

    if (k_shift){
      key  = keymap[code][1];
    }
    else{
      key  = keymap[code][0];
    }
	}


	switch(code){
		case KEY_NUM_LOCK:
			k_num = !(k_num);
			leds = true;
		break;
		case KEY_SCROLL_LOCK:
			k_scroll = !(k_scroll);
			leds = true;
		break;
		case KEY_CAPS_LOCK:
			k_caps = !(k_caps);
			leds = true;
		break;
		case LSHIFT:
		case RSHIFT:
			k_shift = 1;
			return 0;
		case ALT:
			k_alt  = 1;
			return 0;
		case CTRL:
		k_ctrl = 1;
		default:
			if (k_shift){
				key  = keymap[code][1];
			}
			else{
				key  = keymap[code][0];
			}
		break;
	}

	/* Se for alguma tecla relacionada com LED´s devemos atualizar os LED´s. */
	if (leds){
		outportb(0x60, 0xED); /* Envia o comando para atualizar os LED´s. */
		i = 0;
		if (k_scroll){
			i |= 1;
		}
		if (k_num){
			i |= 2;
		}
		if (k_caps){
			i |= 4;
		}
		outportb(0x60, i); /* Envia a nova configuração dos LED´s. */

		return 0;
	}

	return key;
}

int keymap[][2] = {
	/* 0 */		{0, 0},      {ESC, 0},
	/* 2 */		{'1', '!'},  {'2', '@'},
	/* 4 */		{'3', '#'},  {'4', '$'},
	/* 6 */		{'5', '%'},  {'6', '^'},
	/* 8 */		{'7', '&'},  {'8', '*'},
	/* 10 */	{'9', '('},  {'0', ')'},
	/* 12 */	{'-', '_'},  {'=', '+'},
	/* 14 */	{'\b', '\b'},{'\t', 0},
	/* 16 */	{'q', 'Q'},  {'w', 'W'},
	/* 18 */	{'e', 'E'},  {'r', 'R'},
	/* 20 */	{'t', 'T'},  {'y', 'Y'},
	/* 22 */	{'u', 'U'},  {'i', 'I'},
	/* 24 */	{'o', 'O'},  {'p', 'P'},
	/* 26 */	{'�', '`'},  {'[', '{'},
	/* 28 */	{'\n', 0},   {'/', '?'},
	/* 30 */	{'a', 'A'},  {'s', 'S'},
	/* 32 */	{'d', 'D'},  {'f', 'F'},
	/* 34 */	{'g', 'G'},  {'h', 'H'},
	/* 36 */	{'j', 'J'},  {'k', 'K'},
	/* 38 */	{'l', 'L'},  {'�', '�'},
	/* 40 */	{'~', '^'},  {'\'', '"'},
	/* 42 */	{LSHIFT, 0}, {']', '}'},
	/* 44 */	{'z', 'Z'},  {'x', 'X'},
	/* 46 */	{'c', 'C'},  {'v', 'V'},
	/* 48 */	{'b', 'B'},  {'n', 'N'},
	/* 50 */	{'m', 'M'},  {',', '<'},
	/* 52 */	{'.', '>'},  {';', ':'},
	/* 54 */	{RSHIFT, 0}, {0, 0},
	/* 56 */	{ALT, 0},    {' ', ' '},
	/* 58 */	{0, 0},      {KEY_F1, KEY_F1},
	/* 60 */	{KEY_F2, KEY_F2},   {KEY_F3, KEY_F3},
	/* 62 */	{KEY_F4, KEY_F4},   {KEY_F5, KEY_F5},
	/* 64 */	{KEY_F6, KEY_F6},   {KEY_F7, KEY_F7},
	/* 66 */	{KEY_F8, KEY_F8},   {KEY_F9, KEY_F9},
	/* 68 */	{KEY_F10, KEY_F10}, {0, 0},
	/* 70 */	{0, 0},             {KEY_HOME, '7'},
	/* 72 */	{KEY_UP, '8'},      {KEY_PGUP, '9'},
	/* 74 */	{'-', 0},           {KEY_LEFT, '4'},
	/* 76 */	{'5', 0},           {KEY_RIGHT, '6'},
	/* 78 */	{'+', 0},           {KEY_END, '1'},
	/* 80 */	{KEY_DOWN, '2'},    {KEY_PGDN, '3'},
	/* 82 */	{KEY_INS, '0'},     {KEY_DEL, KEY_DEL},
	/* 84 */	{KEY_F11, KEY_F11}, {KEY_F12, KEY_F12},
	/* 86 */	{'\\', '|'},        {'\n', '\n'},
	/* 88 */	{CTRL, 0},          {'/', 0},
	/* 90 */	{SYSRQ, PSCREEN},   {ALT, 0},
	/* 92 */	{KEY_HOME, 0},      {KEY_UP, 0},
	/* 94 */	{KEY_PGUP, 0},      {KEY_LEFT, 0},
	/* 96 */	{KEY_RIGHT, 0},     {KEY_END, 0},
	/* 98 */	{KEY_DOWN, 0},      {KEY_PGDN, 0},
	/* 100 */	{KEY_INS, 0},       {KEY_DEL, 0},
	/* 102 */	{KEY_WIN, 0},       {KEY_WIN, 0},
	/* 104 */	{KEY_MENU, 0}
};
