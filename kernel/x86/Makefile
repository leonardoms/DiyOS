
C_FLAGS = -I ../include -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS =   tables.o memory.o pic.o helper.o

all: clean $(OBJECTS)
	@ar rcs -o ../x86.a $(OBJECTS)

tables.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c tables.c -o tables.o

memory.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c memory.c -o memory.o

pic.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c pic.c -o pic.o

helper.o:
	@echo "[BUILD]"	$@
	@$(ASM) $(ASM_FLAGS) helper.asm -o helper.o

clean:
	@-rm *.o
