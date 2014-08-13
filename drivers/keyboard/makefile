SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin-log -Wimplicit-function-declaration -fno-builtin -nostdinc -I ./include -I ../../libs/mercury/include -I ../../libs/udrcp/include
L_LDFLAGS = -muniverse_i386 -flto -T../link.ld

keyboard.elf:  $(OBJS) ../../libs/udrcp/udrcp.a ../../libs/mercury/libc.a
	@$(LD) $(L_LDFLAGS) -o $@ $^
	cp keyboard.elf ../../build/drivers/keyboard.elf

%.o: %.c
	@$(CC) $(L_CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm keyboard.elf -f

.PHONY: clean

