all: kernel

kernel:
	$(MAKE) -C kernel/src

clean:
	$(MAKE) -C kernel/src clean

.PHONY: all kernel clean
