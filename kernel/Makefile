
C_FLAGS = -I. -Wall -nostdlib -nostdinc -nodefaultlibs -m64 -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS =   start.o x86_tables.o fb.o kb.o io.o pic.o do_it_yourself.o

all: clean $(OBJECTS)
		$(LD) $(LD_FLAGS) -o diyOS_x86 $(OBJECTS) -T x86.ld
		cp diyOS_x86 ../bin/diyOS_x86

start.o:
	$(ASM) $(ASM_FLAGS) start.asm -o start.o

x86_tables.o:
	$(CC) $(C_FLAGS) -c x86_tables.c -o x86_tables.o

fb.o:
	$(CC) $(C_FLAGS) -c fb.c -o fb.o

kb.o:
	$(CC) $(C_FLAGS) -c kb.c -o kb.o

io.o:
	$(CC) $(C_FLAGS) -c io.c -o io.o

pic.o:
	$(CC) $(C_FLAGS) -c pic.c -o pic.o

do_it_yourself.o:
	$(CC) $(C_FLAGS) -c do_it_yourself.c -o do_it_yourself.o

clean:
	-rm *.o
