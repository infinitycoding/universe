SRCS = $(shell find -name '*.c')
SRCS += $(shell find -name '*.asm')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

C_SRCS = $(shell find `pwd` -name '*.c')
C_HDRS = $(shell find `pwd` -name '*.h')
STYLEFLAGS = --style=allman --indent-switches

L_CFLAGS = -Iinclude -Iinclude/util
L_LDFLAGS = -Tlinkerscript.x

ifeq (${ARCH},PPC)
L_CFLAGS +=  -Iinclude/arch/ppc
else ifeq ($(ARCH),i686)
L_CFLAGS +=  -Iinclude/arch/x86
else ifeq ($(ARCH),arm)
L_CFLAGS +=  -Iinclude/arch/arm
endif

kernel32.elf: $(OBJS) linkerscript.x
	@${LD} ${LDFLAGS} $(L_CFLAGS) -o $@ $^
	@cp kernel32.elf $(PREFIX)/kernel32.elf

%.o: %.c
	@${CC} $(L_CFLAGS) ${CFLAGS} -c -o $@ $^

%.ppasm: %.asm
	@${CC} $(L_CFLAGS) ${CFLAGS} -x assembler-with-cpp -P -E -o $@ $^

%.o: %.ppasm
	@${ASM} ${ASFLAGS} -o $@ $^

%.x: %.gen
	@${CC} $(L_CFLAGS) ${CFLAGS} -x assembler-with-cpp -P -E -o $@ $^

style: $(C_SRCS) $(C_HDRS)
	astyle $(STYLEFLAGS) $^

clean:
	@rm -f $(OBJS)
	@rm -f kernel32.elf
	@rm -f linkerscript.x

.PHONY: clean

