SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -fno-builtin-log -Wimplicit-function-declaration -nostdinc -I../mercury/include -I../mercury/include/c/ -Iinclude

all: udrcp.a
	cp -R ./include/* $(PREFIX)/usr/include
	cp udrcp.a $(PREFIX)/usr/lib/

udrcp.a: $(OBJS)
	@ar -rcs $@ $^

%.o: %.c
	@$(CC) $(L_CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm udrcp.a -f

.PHONY: clean

