all: kernel libs drivers user iso-img

# PPC
# I386
# arm
ARCH=I386

CFLAGS = -m32 -Wall -g -nostdinc -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration 
ASFLAGS =  -f elf32
LDFLAGS = -melf_i386


ifeq ($(ARCH),PPC)
QEMU = qemu-system-ppc

else ifeq ($(ARCH),I386)
QEMU = qemu-system-i386 -cdrom cdrom.iso -net nic,model=rtl8139 -net user
CC = gcc
ASM = nasm
LD = ld

else ifeq ($(ARCH),ARM)
CC = arm-linux-gnueabi-gcc
ASM = arm-linux-gnueabi-as
LD = arm-linux-gnueabi-ld
QEMU = qemu-system-arm -cpu arm1176 -M versatilepb -m 256M -nographic -kernel universe.bin
endif

# Export for subfiles 
export CC
export ASM
export LD
export CFLAGS
export ASFLAGS
export LDFLAGS

kernel:
	@$(MAKE) -C kernel ARCH=$(ARCH) 
	@cp kernel/kernel32.elf build/kernel32.elf

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
	@$(MAKE) -C kernel clean
	@$(MAKE) -C user clean
	@$(MAKE) -C libs clean
	@rm *.iso -f

.PHONY: all kernel libs drivers user clean qemu
