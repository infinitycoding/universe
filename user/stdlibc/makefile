SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -fno-builtin-log -Wimplicit-function-declaration -nostdinc -I ../include

stdlibc.a: $(OBJS)
	@ar -rcs $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS)
	@rm stdlibc.a

.PHONY: clean

