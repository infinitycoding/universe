SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -fno-builtin-log -Wimplicit-function-declaration -nostdinc -I ../mercury/include -I include

all: pfp.a
	cp -R ./include/* $(PREFIX)/usr/include
	cp pfp.a $(PREFIX)/usr/lib/

pfp.a: $(OBJS)
	@ar -rcs $@ $^

%.o: %.c
	@$(CC) $(L_CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm pfp.a -f

.PHONY: clean

