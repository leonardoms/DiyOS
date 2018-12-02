
.PHONY: all clean

KERNEL := DiyOS
ARCH := x86
KERNEL_NAME := $(KERNEL)_$(ARCH)

AUXFILES := Makefile Readme.md
PROJDIRS := drivers gfx gui kernel lib

SRCFILES := $(shell find $(PROJDIRS) -type f -name \*.c)
HDRFILES := $(shell find $(PROJDIRS) -type f -name \*.h)
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
ASMFILES := $(shell find $(PROJDIRS) -type f -name \*.asm)
ASMOBJFILES := $(patsubst %.asm,%.o,$(ASMFILES))

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
CFLAGS := -g $(WARNINGS) -I include/ -nostdlib -nostdinc -nodefaultlibs -ffreestanding -m32 -D$(ARCH)

ifeq ($(ARCH),x86)
        LDFLAGS := -m elf_i386
				NASM_FLAGS := -f elf32
else
        LDFLAGS :=
				NASM_FLAGS :=
endif

run: iso
	qemu-system-i386 -boot d -cdrom build/$(KERNEL_NAME)-live.iso -vga std -m 64

iso: all
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o ./build/$(KERNEL_NAME)-live.iso ./bin

all: $(OBJFILES) $(ASMOBJFILES)
	@echo "linking $(KERNEL_NAME)"
	@$(LD) $(LDFLAGS) -o bin/$(KERNEL_NAME) $(OBJFILES) $(ASMOBJFILES) -T kernel/$(ARCH)/linker.ld > /dev/null

%.o: %.asm
	nasm $< -o $@ $(NASM_FLAGS) > /dev/null

clean:
	@-rm -rf $(OBJFILES) $(ASMOBJFILES) bin/$(KERNEL_NAME) build/$(KERNEL_NAME)-live.iso
