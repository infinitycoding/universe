all: kernel libs drivers user iso-img

# PPC
# I386
# ARM
ARCH=I386

ifeq ($(ARCH),PPC)
QEMU = qemu-system-ppc
else ifeq ($(ARCH),I386)
QEMU = qemu-system-i386 -cdrom cdrom.iso -net nic,model=rtl8139 -net user
else ifeq ($(ARCH),ARM)
QEMU = qemu-system-arm -cpu arm1176 -M versatilepb -m 256M -nographic -kernel universe.bin
endif

kernel:
	@$(MAKE) -C kernel/src ARCH=$(ARCH)
	@cp kernel/src/kernel32.elf build/kernel32.elf

libs:
	@$(MAKE) -C libs ARCH=$(ARCH)

drivers:libs
	@$(MAKE) -C drivers ARCH=$(ARCH)

user: libs
	@$(MAKE) -C user ARCH=$(ARCH)
	@cp user/ultrashell/ultrashell.elf build/ultrashell.elf
	@cp user/test/test.elf build/test.elf

iso-img:
	@genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf-8 -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso build

usb:
	mount /dev/sdb /mnt
	cp -R ./build /mnt
	grub-install --root-directory=/mnt --no-floppy --recheck /dev/sdb

qemu: kernel libs drivers user iso-img
	$(QEMU) 

clean:
	@$(MAKE) -C kernel/src clean
	@$(MAKE) -C user clean
	@$(MAKE) -C libs clean
	@rm *.iso -f

.PHONY: all kernel libs drivers user clean qemu
