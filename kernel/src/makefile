SRCS = $(shell find -name '*.c')
SRCS += $(shell find -name '*.asm')
OBJS = $(addsuffix .o,$(basename $(SRCS)))


CFLAGS += -I ../include 
LDFLAGS += -Tlinkerscript.x

ifeq ($(ARCH),PPC)
CFLAGS +=  -I ../include/arch/ppc
else ifeq ($(ARCH),I386)
CFLAGS +=  -I ../include/arch/x86
else ifeq ($(ARCH),arm)
CFLAGS +=  -I ../include/arch/arm
endif




kernel32.elf: $(OBJS) linkerscript.x
	@$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $^

%.ppasm: %.asm
	@$(CC) $(CFLAGS) -x assembler-with-cpp -P -E -o $@ $^

%.o: %.ppasm
	@$(ASM) $(ASFLAGS) -o $@ $^

%.x: %.gen
	@$(CC) $(CFLAGS) -x assembler-with-cpp -P -E -o $@ $^

clean:
	@rm -f $(OBJS)
	@rm -f kernel32.elf
	@rm -f linkerscript.x

.PHONY: clean
