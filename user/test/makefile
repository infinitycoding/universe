SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration -fno-builtin -nostdinc -I include -I ../../libs/udrcp/include -I ../../libs/mercury/include
L_LDFLAGS = -muniverse_i386 -flto -T../link.ld

test: $(OBJS) ../../libs/mercury/crt/crt0.o ../../libs/udrcp/udrcp.a ../../libs/mercury/libc.a
	@$(LD) $(L_LDFLAGS) -o $@  $^
	cp test $(PREFIX)/test

%.o: %.c
	@$(CC) $(L_CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm test -f

.PHONY: clean

