SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

parser.a: $(OBJS)
	ar -rcs parser.a $^

%.o: %.c
	gcc -m32 -Wall -I ../include -c -o $@ $^
	
clean:
	@rm $(OBJS) -f
	@rm parser.a -f
