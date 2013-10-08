all: kernel user iso-img

kernel:
	$(MAKE) -C kernel/src
	cp kernel/src/kernel32.elf build/kernel32.elf

ultrashell:
	$(MAKE) -C user/ultrashell
	cp user/ultrashell/ultrashell.elf build/ultrashell.elf

stdlibc:
	$(MAKE) -C user/stdlibc

user: stdlibc
	$(MAKE) -C user
	$(MAKE) -C user/ultrashell
	cp user/ultrashell/ultrashell.elf build/ultrashell.elf
	cp user/test/test.elf build/test.elf

iso-img:
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso build

qemu: kernel user iso-img
	qemu-system-i386 -cdrom cdrom.iso -net nic,model=rtl8139 -net user

clean:
	$(MAKE) -C kernel/src clean
	$(MAKE) -C user clean
	$(MAKE) -C user/stdlibc clean
	$(MAKE) -C user/ultrashell clean



.PHONY: all kernel stdlibc user clean qemu

