
C_FLAGS = -I../include -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

C_FILES = itoa.c atoi.c memcpy.c memset.c printf.c strcpy.c strlen.c
OBJECTS = $(C_FILES:.c=.o)

.PHONY: all

all: clean $(OBJECTS)
	@ar rcs -o ../small.a $(OBJECTS)

.c.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c $< -o $@

.PHONY: clean

clean:
	@-rm *.o > /dev/null
