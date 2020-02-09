SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -nostdinc -I ../../libs/mercury/include -I ../../libs/mercury/include/c/ -I ../../libs/mercury/include/sys/universe  -I include
L_LDFLAGS = -muniverse_i386 -flto -T ../link.ld

cp: ../../libs/mercury/sys/universe/$(ARCH)/crt/crt0.o $(OBJS) ../../libs/mercury/universe_libc.a
	@$(LD) $(L_LDFLAGS) -o $@ $^
	cp cp $(PREFIX)/cp

host: 
	gcc -I include -o cp $(SRCS)

%.o: %.c
	@$(CC) $(L_CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm cp -f

.PHONY: clean




