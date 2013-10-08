SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration -fno-builtin -nostdinc -I ../../libs/stdlibc/include
LDFLAGS = -melf_i386 -flto -T../link.ld

test.elf:  ../entry.o $(OBJS) ../../libs/stdlibc/stdlibc.a
	@$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm test.elf -f

.PHONY: clean

