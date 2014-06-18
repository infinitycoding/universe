SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration -fno-builtin -nostdinc -I include -I ../../libs/libc/include -I ../../libs/udrcp/include
LDFLAGS = -muniverse_i386 -flto -T../link.ld

test.elf: ../../libs/crt/entry.o $(OBJS) ../../libs/udrcp/udrcp.a  ../../libs/libc/libc.a
	@$(LD) $(LDFLAGS) -o $@ $^
	cp test.elf $(PREFIX)/test.elf

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm test.elf -f

.PHONY: clean

