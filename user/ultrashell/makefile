SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -nostdinc -I ../include -I include
LDFLAGS = -melf_i386 -flto -T ../link.ld

ultrashell.elf: ../entry.o $(OBJS) ../stdlibc/stdlibc.a
	@$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS)
	@rm ultrashell.elf

.PHONY: clean




