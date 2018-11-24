
C_FLAGS = -I../../include -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -m32
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

C_FILES = mm.c frame.c page.c
OBJECTS = $(C_FILES:.c=.o)

.PHONY: all clean

all: clean $(OBJECTS)
	# @ar rcs -o ../mm.a $(OBJECTS)

.c.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c $< -o $@

clean:
	@-rm *.o > /dev/null
