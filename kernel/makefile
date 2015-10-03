
C_SRCS = $(shell find `pwd` -name '*.c')
C_HDRS = $(shell find `pwd` -name '*.h')
STYLEFLAGS = --style=allman --indent-switches

L_LDFLAGS = -Tlinkerscript.x


ifeq (${ARCH}, i686)
L_CFLAGS += -Iarch/x86/include -D _ARCH_i386_
SRCS += $(shell find -iname '*.c' -path './arch/x86/*')
SRCS += $(shell find -iname '*.asm' -path './arch/x86/*')
else ifeq (${ARCH}, arm)
L_CFLAGS += -Iarch/arm/include -D _ARCH_arm_
SRCS += $(shell find -iname '*.c' -path './arch/arm/*')
SRCS += $(shell find -iname '*.asm' -path './arch/arm/*')
endif

ifeq (${PLATFORM},pc)
L_CFLAGS += -Iplatform/pc/include
SRCS += $(shell find -iname '*.c' -path './platform/pc/*')
SRCS += $(shell find -iname '*.asm' -path './platform/pc/*')
endif

OBJS = $(addsuffix .o,$(basename $(SRCS)))
MODS = $(shell ./modlst.sh)

all: linkerscript.x #kernel32.elf
	$(MAKE) -C ./modules MODULES="$(MODS)" ARCH="$(ARCH)" PLATFORM="$(PLATFORM)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)" CFLAGS="$(CFLAGS)" ASFLAGS="$(ASFLAGS)" LDFLAGS="$(LDFLAGS)"

kernel32.elf: $(OBJS)
	${LD} $(L_LDFLAGS) ${LDFLAGS} -o $@ $^
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
	@rm -f $(shell find `pwd` -name '*.o') $(shell find `pwd` -name '*.a')
	@rm -f kernel32.elf
	@rm -f linkerscript.x

.PHONY: clean
