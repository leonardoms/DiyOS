
C_FLAGS = -I ../include -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS =   main.o

all: clean $(OBJECTS)
	cp ../lib/x86_user.ld x86_user.ld
	cp ../lib/start.o start.o
	$(LD) $(LD_FLAGS) -T x86_user.ld $(OBJECTS) start.o -o gpf.elf

main.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c main.c -o main.o

clean:
	@-rm *.o
