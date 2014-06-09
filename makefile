all: kernel libs drivers user iso-img

# PPC
# I386
# arm
ARCH=I386
CFLAGS = -Wall -g -nostdinc -fno-stack-protector -fno-builtin-log
ASFLAGS =  -f elf32
LDFLAGS = -muniverse_i386

PREFIX = $(PWD)/build
#USBDEV = /dev/sdb

ifeq ($(ARCH),PPC)
QEMU = qemu-system-ppc

else ifeq ($(ARCH),I386)
QEMU = qemu-system-i386 -cdrom cdrom.iso -net nic,model=rtl8139 -net user
CC = i686-universe-gcc
ASM = nasm
LD = i686-universe-ld 

else ifeq ($(ARCH),arm)
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
export PREFIX


kernel:
	@$(MAKE) -C kernel ARCH=$(ARCH)

libs:
	@$(MAKE) -C libs ARCH=$(ARCH)

drivers:libs
	@$(MAKE) -C drivers ARCH=$(ARCH)

user: libs
	@$(MAKE) -C user ARCH=$(ARCH)

iso-img:
	@genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf-8 -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso build

usb:
	mount $(USBDEV) /mnt
	cp -R ./build /mnt
	grub-install --root-directory=/mnt --no-floppy --recheck $(USBDEV)

qemu: kernel libs drivers user iso-img
	$(QEMU)

debug: kernel libs drivers user iso-img
	$(QEMU) -s -S

clean:
	@$(MAKE) -C kernel clean
	@$(MAKE) -C drivers clean
	@$(MAKE) -C user clean
	@$(MAKE) -C libs clean
	@rm *.iso -f

.PHONY: all kernel libs drivers user clean qemu
