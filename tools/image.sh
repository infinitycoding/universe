#!bin/sh
sudo mount -o loop floppy.img /media/tmp
sudo cp kernel/kernel32.elf /media/tmp/kernel32.elf
sudo umount /media/tmp
