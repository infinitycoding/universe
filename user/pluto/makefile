SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration -fno-builtin -nostdinc -I include -I ../../libs/udrcp/include -I ../../libs/mercury/include -I../../libs/mercury/include/c/ -I ../../libs/mercury/include/sys/universe 
L_LDFLAGS = -melf_i386 -flto -T../link.ld

pluto: $(OBJS) ../../libs/mercury/sys/universe/$(ARCH)/crt/crt0.o ../../libs/udrcp/udrcp.a ../../libs/mercury/universe_libc.a
	@$(LD) $(L_LDFLAGS) -o $@  $^
	cp pluto $(PREFIX)/pluto

%.o: %.c
	$(CC) $(L_CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm test -f

.PHONY: clean

