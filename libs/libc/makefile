SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -fno-builtin-log -Wimplicit-function-declaration -nostdinc -I include

all: libc.a
	cp -R ./include/* $(PREFIX)/usr/include
	cp libc.a $(PREFIX)/usr/lib/

libc.a: $(OBJS)
	@ar -rcs $@ $^

%.o: %.c
	@${CC} ${CFLAGS} -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm libc.a -f

.PHONY: clean

