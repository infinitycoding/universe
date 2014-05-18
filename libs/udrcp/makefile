SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -fno-builtin-log -Wimplicit-function-declaration -nostdinc -I ../libc/include -I include

all: udrcp.a
	cp -R ./include/* ../../build/usr/include
	cp udrcp.a ../../build/usr/lib/

udrcp.a: $(OBJS)
	@ar -rcs $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm udrcp.a -f

.PHONY: clean

