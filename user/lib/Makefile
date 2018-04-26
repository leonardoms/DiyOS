
C_FLAGS = -I ../include -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS =   puts.o putchar.o

all: clean $(OBJECTS) start.o
	@ar rcs -o ulib.a $(OBJECTS)

start.o:
	@echo "[BUILD]"	$@
	@$(ASM) $(ASM_FLAGS) start.asm -o start.o

putchar.o:
	@echo "[BUILD]"	$@
	@$(ASM) $(ASM_FLAGS) putchar.asm -o putchar.o

puts.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c puts.c -o puts.o

clean:
	@-rm *.o
