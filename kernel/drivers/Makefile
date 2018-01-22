
C_FLAGS = -I../include -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS =   kb.o fb.o timer.o

all: clean $(OBJECTS)
	@ar rcs -o ../built-in_drivers.a $(OBJECTS)

fb.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c fb.c -o fb.o

kb.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c kb.c -o kb.o

timer.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c timer.c -o timer.o

clean:
	@-rm *.o > /dev/null
