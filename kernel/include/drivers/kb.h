#ifndef _KB_H_
#define _KB_H_

#include <x86/x86.h>
#include <io.h>
#include <drivers/fb.h>

void setup_kb();
int32_t convert(uint32_t code); // convert keycode to ascii

int keymap[105][2];

#define KEY_RELEASED 0x80
#define KEYMAP_SIZE (sizeof(keymap) / sizeof(keymap[0][0]))

#define ESC 0x1B

#define KEY_F1  0x80
#define KEY_F2  (KEY_F1 + 1)
#define KEY_F3  (KEY_F2 + 1)
#define KEY_F4  (KEY_F3 + 1)
#define KEY_F5  (KEY_F4 + 1)
#define KEY_F6  (KEY_F5 + 1)
#define KEY_F7  (KEY_F6 + 1)
#define KEY_F8  (KEY_F7 + 1)
#define KEY_F9  (KEY_F8 + 1)
#define KEY_F10 (KEY_F9 + 1)
#define KEY_F11 (KEY_F10 + 1)
#define KEY_F12 (KEY_F11 + 1)

#define LSHIFT 	0x2A
#define RSHIFT 	0x36
#define CTRL   	0x1D
#define ALT    	0x38
#define DEL    	0x53

#define KEY_INS   0x90
#define KEY_DEL   (KEY_INS + 1)
#define KEY_HOME  (KEY_DEL + 1)
#define KEY_END   (KEY_HOME + 1)
#define KEY_PGUP  (KEY_END + 1)
#define KEY_PGDN  (KEY_PGUP + 1)
#define KEY_LEFT  (KEY_PGDN + 1)
#define KEY_UP    (KEY_LEFT + 1)
#define KEY_DOWN  (KEY_UP + 1)
#define KEY_RIGHT (KEY_DOWN + 1)

#define SYSRQ    0x70
#define PSCREEN  (SYSRQ + 1)
#define KEY_WIN  (PSCREEN + 1)
#define KEY_MENU (KEY_WIN + 1)

#define	KEY_NUM_LOCK		0x45
#define	KEY_SCROLL_LOCK		0x46
#define	KEY_CAPS_LOCK		0x3A

#endif
