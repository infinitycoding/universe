SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -fno-stack-protector -fno-builtin -fno-builtin-log -Wimplicit-function-declaration -nostdinc -I include -I ../libc/include -I ../udrcp/include

cdi.a: $(OBJS)
	@ar -rcs $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@rm $(OBJS) -f
	@rm *.a -f

.PHONY: clean

