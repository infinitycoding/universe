SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -nostdinc -I ../../../libs/mercury/include -I ../include
L_LDFLAGS = -muniverse_i386 -flto -T link.ld

all: test host

test:  $(OBJS)
	@$(LD) $(L_LDFLAGS) -o $@ ../../../libs/mercury/crt/crt0.o $^  ../lib/parser.a ../../../libs/mercury/libc.a

host:
	gcc -m32 -I ../include -o host_test $(SRCS) ../lib/parser.a

%.o: %.c
	@$(CC) $(L_CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm test -f

.PHONY: clean