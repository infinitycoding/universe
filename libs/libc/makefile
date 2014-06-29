SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

L_CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -fno-builtin-log -Wimplicit-function-declaration -nostdinc -I include

all: libc.a

libc.a: $(OBJS)
	@ar -rcs $@ $^

%.o: %.c
	@${CC} $(L_CFLAGS) ${CFLAGS} -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm libc.a -f

.PHONY: clean

