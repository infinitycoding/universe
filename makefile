#all: kernel libs drivers user iso-img
all: kernel iso-img

# PPC
# I686
# arm
ARCH=i686
PLATFORM=pc
HOSTCOMPILER=true
HOST_PREFIX=/usr/

CFLAGS = -Wall -g -nostdinc -fno-stack-protector -fno-builtin-log
ASFLAGS = -felf32
LDFLAGS = -muniverse_i386

PREFIX = $(PWD)/build
#USBDEV = /dev/sdb

CC = $(ARCH)-universe-gcc
LD = $(ARCH)-universe-ld
ASM = $(ARCH)-universe-as
CCPP = $(ARCH)-universe-g++


#change defaults
ifeq ($(ARCH),PPC)
QEMU = qemu-system-ppc

else ifeq ($(ARCH),i686)
QEMU = qemu-system-i386 -cdrom cdrom.iso -net nic,model=rtl8139 -net user
ASM = nasm
ifeq ($(HOSTCOMPILER),true)
CFLAGS = -m32 -Wall -g -nostdinc -fno-stack-protector -fno-builtin-log
ASFLAGS = -felf32
LDFLAGS = -melf_i386
CC = gcc
LD = ld
CCPP = g++
endif
else ifeq ($(ARCH),arm)
CC = arm-universe-eabi-gcc
LD = arm-universe-eabi-ld
ASM = arm-universe-eabi-as
QEMU = qemu-system-arm -cpu arm1176 -M versatilepb -m 256M -nographic -kernel universe.bin
endif


export PREFIX


kernel:
	@$(MAKE) -C kernel ARCH="$(ARCH)" PLATFORM="$(PLATFORM)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)" CFLAGS="$(CFLAGS)" ASFLAGS="$(ASFLAGS)" LDFLAGS="$(LDFLAGS)"

libs:
	@$(MAKE) -C libs ARCH="$(ARCH)" PLATFORM="$(PLATFORM)" CCPP="$(CCPP)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)" ASFLAGS="$(ASFLAGS)" LDFLAGS="$(LDFLAGS)"

drivers:libs
	@$(MAKE) -C drivers ARCH="$(ARCH)" PLATFORM="$(PLATFORM)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)" CFLAGS="$(CFLAGS)" ASFLAGS="$(ASFLAGS)" LDFLAGS="$(LDFLAGS)"

user: libs
	@$(MAKE) -C user ARCH="$(ARCH)" PLATFORM="$(PLATFORM)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)" CFLAGS="$(CFLAGS)" ASFLAGS="$(ASFLAGS)" LDFLAGS="$(LDFLAGS)"

iso-img:
	@genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf-8 -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso build

usb:
	mount $(USBDEV) /mnt
	cp -R ./build /mnt
	grub-install --root-directory=/mnt --no-floppy --recheck $(USBDEV)

qemu: all
	$(QEMU)

debug: all
	$(QEMU) -s -S

style:
	$(MAKE) -C kernel style

update:
	git pull origin master
	git submodule update

clean:
	@$(MAKE) -C kernel clean ARCH="$(ARCH)" PLATFORM="$(PLATFORM)"
	@$(MAKE) -C drivers clean ARCH="$(ARCH)" PLATFORM="$(PLATFORM)"
	@$(MAKE) -C user clean ARCH="$(ARCH)" PLATFORM="$(PLATFORM)"
	@$(MAKE) -C libs clean ARCH="$(ARCH)" PLATFORM="$(PLATFORM)"
	@rm *.iso -f

dependencies:
	git submodule init
	git submodule update
	if [ ! -f $(HOST_PREFIX)/bin/$(ARCH)-universe-gcc ]; then \
		$(MAKE) -C cc HOST_PREFIX="$(HOST_PREFIX)" TARGET_ARCH="$(ARCH)"; \
		cd cc; \
		sudo make install; \
		cd .. ; \
	fi


.PHONY: all kernel libs drivers user clean qemu
