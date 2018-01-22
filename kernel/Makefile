
C_FLAGS = -I include/ -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS = start.o art.o io.o do_it_yourself.o

all: clean x86.a built-in_drivers.a small.a $(OBJECTS)
		$(LD) $(LD_FLAGS) -o diyOS_x86 $(OBJECTS) x86.a built-in_drivers.a small.a -T x86/x86.ld
		cp diyOS_x86 ../bin/diyOS_x86

small.a:
	@echo "[BUILD BASIC LIB]"	$@
	@make -C ./libsmall/

x86.a:
	@echo "[BUILD ARCH]"	$@
	@make -C ./x86/

built-in_drivers.a:
	@echo "[BUILD DRIVERS]"	$@
	@make -C ./drivers/

start.o:
	@echo "[BUILD]"	$@
	@$(ASM) $(ASM_FLAGS) start.asm -o start.o

io.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c io.c -o io.o

art.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c art.c -o art.o

do_it_yourself.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c do_it_yourself.c -o do_it_yourself.o

clean:
	-rm *.o *.a
