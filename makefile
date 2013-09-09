all: kernel user iso-img

kernel:
	$(MAKE) -C kernel/src
	cp kernel/src/kernel32.elf build/kernel32.elf

user:
	$(MAKE) -C user/test
	cp user/test/test.elf build/test.elf

iso-img:
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso build

qemu: kernel user iso-img
	qemu-system-i386 -cdrom cdrom.iso

clean:
	$(MAKE) -C kernel/src clean
	$(MAKE) -C user/test clean

.PHONY: all kernel user clean qemu

