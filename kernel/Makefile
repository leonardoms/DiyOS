
C_FLAGS = -I include/ -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS = test.o art.o bios.o io.o pci.o do_it_yourself.o

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

test.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c test.c -o test.o

bios.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c bios.c -o bios.o

io.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c io.c -o io.o

pci.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c pci.c -o pci.o

art.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c art.c -o art.o

do_it_yourself.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c do_it_yourself.c -o do_it_yourself.o

clean:
	-rm *.o *.a
