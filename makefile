all: kernel user iso-img

kernel:
    @$(MAKE) -C kernel/src
    @cp kernel/src/kernel32.elf build/kernel32.elf

libs:
    @$(MAKE) -C libs

user: libs
    @$(MAKE) -C user
    @cp user/ultrashell/ultrashell.elf build/ultrashell.elf
    @cp user/test/test.elf build/test.elf

iso-img:
    @genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf-8 -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso build

qemu: kernel user iso-img
    qemu-system-i386 -cdrom cdrom.iso -net nic,model=rtl8139 -net user

clean:
    @$(MAKE) -C kernel/src clean
    @$(MAKE) -C user clean
    @$(MAKE) -C libs clean

.PHONY: all kernel libs user clean qemu
