SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration -fno-builtin -nostdinc -I include -I ../../libs/libc/include -I ../../libs/udrcp/include
LDFLAGS = -muniverse_i386 -flto -T/usr/i686-universe/lib/ldscripts/universe_i386.x

test.elf: $(OBJS)
	@$(LD) $(LDFLAGS) -o $@ $^  ../../libs/udrcp/udrcp.a  /usr/i686-universe/lib/libc.a /usr/i686-universe/lib/libnosys.a
	cp test.elf $(PREFIX)/test.elf

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm test.elf -f

.PHONY: clean

