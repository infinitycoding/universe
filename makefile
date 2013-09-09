all: kernel user

kernel:
	$(MAKE) -C kernel/src

user:
	$(MAKE) -C user/test

qemu: kernel user
	qemu-system-i386 -kernel kernel/src/kernel32.elf -initrd user/test/test.elf

clean:
	$(MAKE) -C kernel/src clean
	$(MAKE) -C user/test clean

.PHONY: all kernel user clean qemu

