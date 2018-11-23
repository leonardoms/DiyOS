
C_FLAGS = -I include/ -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

C_FILES = message.c module.c test.c gfx.c task_queue.c queue.c panic.c bios.c io.c pci.c aspace.c task.c elf.c syscall.c terminal.c do_it_yourself.c
OBJECTS = $(C_FILES:.c=.o)

.PHONY: all clean

all: clean x86.a built-in_drivers.a small.a gui.a $(OBJECTS)
	$(LD) $(LD_FLAGS) -o diyOS_x86 $(OBJECTS) x86.a built-in_drivers.a small.a gui.a -T x86/x86.ld
	cp diyOS_x86 ../bin/diyOS_x86

small.a:
	@echo "[BUILD BASIC LIB]"	$@
	@make -C ./libsmall/

gui.a:
	@echo "[BUILD GUI]"	$@
	@make -C ./gui/

x86.a:
	@echo "[BUILD ARCH]"	$@
	@make -C ./x86/

built-in_drivers.a:
	@echo "[BUILD DRIVERS]"	$@
	@make -C ./drivers/

.c.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c $< -o $@

clean:
	@-rm *.o > /dev/null
