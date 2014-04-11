SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -fno-builtin-log -Wimplicit-function-declaration -nostdinc -I include
ASM = nasm
ASFLAGS =  -f elf32

crt: $(OBJS)
	@nasm -f elf32 -o entry.o entry.asm
	@nasm -f elf32 -o crt0.o crt0.asm 

%.o: %.c @$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f

.PHONY: clean

