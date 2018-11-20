
C_FLAGS = -I ../../include -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS =   frame.o kmalloc.o

all: clean $(OBJECTS)

frame.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c frame.c -o frame.o

kmalloc.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c kmalloc.c -o kmalloc.o

clean:
	@-rm *.o
