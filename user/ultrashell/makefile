SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -nostdinc -I ../../libs/libc/include -I include
LDFLAGS = -melf_i386 -flto -T ../link.ld

ultrashell.elf: ../../libs/crt/entry.o $(OBJS) ../../libs/libc/libc.a
	@$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm ultrashell.elf -f

.PHONY: clean




