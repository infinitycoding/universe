SRCS = $(shell find -name '*.c')
SRCS += $(shell find -name '*.asm')
OBJS = $(addsuffix .o,$(basename $(SRCS)))


CFLAGS += -Iinclude -Iinclude/util
LDFLAGS += -Tlinkerscript.x

ifeq ($(ARCH),PPC)
CFLAGS +=  -Iinclude/arch/ppc
else ifeq ($(ARCH),I386)
CFLAGS +=  -Iinclude/arch/x86
else ifeq ($(ARCH),arm)
CFLAGS +=  -Iinclude/arch/arm
endif




kernel32.elf: $(OBJS) linkerscript.x
	@$(LD) $(LDFLAGS) -o $@ $^
	@cp kernel/kernel32.elf $(PREFIX)/kernel32.elf

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
