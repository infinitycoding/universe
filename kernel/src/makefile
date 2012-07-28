SRCS = $(shell find -name '*.c')
SRCS += $(shell find -name '*.asm')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
ASM = nasm
LD = ld

ASFLAGS =  -f elf32
CFLAGS = -m32 -Wall -g -nostdinc -fno-stack-protector -I ../include
LDFLAGS = -melf_i386 -Tkernel.ld

kernel32.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.asm
	$(ASM) $(ASFLAGS) -o $@ $^

clean:
	rm $(OBJS)
	rm kernel32.elf

.PHONY: clean
