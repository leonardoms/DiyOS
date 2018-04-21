
C_FLAGS = -I../include -Wall -nostdlib -nostdinc -nodefaultlibs -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -m32 # -fkeep-inline-functions
CC = gcc
ASM = nasm
ASM_FLAGS = -f elf32
LD = ld
LD_FLAGS =  -m elf_i386

OBJECTS =   kb.o fb.o timer.o bochs_vbe.o ne2000.o rtl81xx.o rtl8139.o

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

bochs_vbe.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c bochs_vbe.c -o bochs_vbe.o

ne2000.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c ne2000.c -o ne2000.o

rtl81xx.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c rtl81xx.c -o rtl81xx.o

rtl8139.o:
	@echo "[BUILD]"	$@
	@$(CC) $(C_FLAGS) -c rtl8139.c -o rtl8139.o	

clean:
	@-rm *.o > /dev/null
