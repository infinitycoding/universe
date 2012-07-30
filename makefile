all: kernel

kernel:
	$(MAKE) -C kernel/src

qemu: kernel
	qemu-i386 --kernel kernel/src/kernel32.elf

clean:
	$(MAKE) -C kernel/src clean

.PHONY: all kernel clean qemu
