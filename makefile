all: kernel

kernel:
	$(MAKE) -C kernel/src

qemu: kernel
	qemu --kernel kernel/src/kernel32.elf

clean:
	$(MAKE) -C kernel/src clean

.PHONY: all kernel clean qemu
