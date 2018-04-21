all:
	make -C kernel

iso:
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o ./build/diyOS_x86-live.iso ./bin

run:
	bochs -f bochsrc.bxrc

run-debug:
	bochs -f bochsrc_debug.bxrc

run-qemu:
	qemu-system-i386 -boot d -cdrom build/diyOS_x86-live.iso -vga std -m 32 -netdev tap,id=hn0,ifname=tap0,script=no,downscript=no -net nic,model=rtl8139,netdev=hn0,id=nic1 #,mac=de:ad:00:00:be:ef #ne2k_pci
