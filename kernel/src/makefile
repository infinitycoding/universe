SRCS = $(shell find -name '*.c')
SRCS += $(shell find -name '*.asm')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
ASM = nasm
LD = ld

ASFLAGS =  -f elf32
CFLAGS = -m32 -Wall -g -nostdinc -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration -I ../include
LDFLAGS = -melf_i386 -Tlinkerscript.x

kernel32.elf: $(OBJS) linkerscript.x
	@$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

%.ppasm: %.asm
	@$(CC) $(CFLAGS) -x assembler-with-cpp -P -E -o $@ $^

%.o: %.ppasm
	@$(ASM) $(ASFLAGS) -o $@ $^

%.x: %.gen
	@$(CC) $(CFLAGS) -x assembler-with-cpp -P -E -o $@ $^

clean:
	@rm -f $(OBJS)
	@rm -f kernel32.elf

.PHONY: clean
