SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration -fno-builtin -nostdinc -I ./include -I ../../libs/mercury/include -I ../../libs/udrcp/include
L_LDFLAGS = -muniverse_i386 -flto -T../link.ld

cga.elf:  ../../libs/mercury/crt/crt0.o  $(OBJS) ../../libs/udrcp/udrcp.a ../../libs/mercury/libc.a
	@$(LD) $(L_LDFLAGS) -o $@ $^
	cp cga.elf ../../build/drivers/cga.elf

%.o: %.c
	@$(CC) $(L_CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm cga.elf -f

.PHONY: clean

