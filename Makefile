
.PHONY: all clean

KERNEL := DiyOS
ARCH := x86
WARNING := no
KERNEL_NAME := $(KERNEL)_$(ARCH)

AUXFILES := Makefile Readme.md
PROJDIRS := drivers gfx gui kernel lib

SRCFILES := $(shell find $(PROJDIRS) -type f -name \*.c)
HDRFILES := $(shell find $(PROJDIRS) -type f -name \*.h)
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
ASMFILES := $(shell find $(PROJDIRS) -type f -name \*.asm)
ASMOBJFILES := $(patsubst %.asm,%.o,$(ASMFILES))

# https://wiki.osdev.org/GCC_Cross-Compiler
CC := gcc
LD := ld
AR := ar
AS := as

ifeq ($(WARNING),yes)
	WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
	            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
	            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
	            -Wconversion -Wstrict-prototypes
	ASMWARNINGS := -W-all
else
	WARNINGS := -w
	ASMWARNINGS := -w-all
endif

CFLAGS := -g $(WARNINGS) -D$(ARCH) -I include/ -mno-sse -nostdlib -nostdinc -nodefaultlibs -ffreestanding -m32 #-mpreferred-stack-boundary=2

ifeq ($(ARCH),x86)
        LDFLAGS := -m elf_i386
				NASM_FLAGS := -f elf32
else
        LDFLAGS :=
				NASM_FLAGS :=
endif

run: iso
	@echo "[RUN  ] ..."
	@qemu-system-i386 -boot d -cdrom build/$(KERNEL_NAME)-live.iso -vga std -m 128 -serial file:serial_out.txt -no-acpi

iso: all
	@echo "[ISO  ] build/$(KERNEL_NAME)-live.iso"
	@genisoimage -quiet -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o ./build/$(KERNEL_NAME)-live.iso ./bin > /dev/null


all: $(OBJFILES) $(ASMOBJFILES)
	@echo "[LINK ] bin/$(KERNEL_NAME)"
	@$(LD) $(LDFLAGS) -o bin/$(KERNEL_NAME) $(OBJFILES) $(ASMOBJFILES) -T kernel/$(ARCH)/linker.ld > /dev/null

%.o: %.c
	@echo "[BUILD] $<"
	@$(CC) -c $(CFLAGS) $(WARNINGS) -o $@ $<

%.o: %.asm
	@echo "[BUILD] $<"
	@nasm $< -o $@ $(NASM_FLAGS) $(ASMWARNINGS)

clean:
	@-rm -rf $(OBJFILES) $(ASMOBJFILES) bin/$(KERNEL_NAME) build/$(KERNEL_NAME)-live.iso
