
MODFILE =$(shell basename `pwd`).a
C_SRCS = $(shell find src/ -name  '*.c')
ASM_SRCS = $(shell find src/ -name '*.asm')
C_OBJS = $(addsuffix .o,$(basename $(C_SRCS)))
ASM_OBJS = $(addsuffix .o,$(basename $(ASM_SRCS)))

ifneq ($(wildcard src/arch/.),)
ifeq ($(wildcard src/arch/$(ARCH)/.),)
$(error $(MODNAME) This module is not $(ARCH) compatible)
else
C_SRCS += $(shell find src/arch/$(ARCH)/ -name  '*.c')
ASM_SRCS += $(shell find src/arch/$(ARCH)/ -name '*.asm')
endif
endif


all: $(MODFILE) test
test: testmod run_test
build: $(MODFILE)
$(MODFILE): $(C_OBJS) $(ASM_OBJS)
	ar -rcs $@ $^

%.o: %.c $(C_SRCS)
	$(CC) ${CFLAGS} $(L_CFLAGS) -c -o $@ $< $

%.o: %.asm $(ASM_SRCS)
	$(ASM) $(ASFLAGS) -o $@ $< $


TEST_C_SRCS = $(shell find unittest/ -name '*.c')
TEST_ASM_SRCS = $(shell find unittest/ -name '*.asm')
TEST_C_OBJS = $(addsuffix .o,$(basename $(TEST_C_SRCS)))
TEST_ASM_OBJS = $(addsuffix .o,$(basename $(TEST_ASM_SRCS)))

ifneq ($(wildcard unittest/arch/.),)
ifeq ($(wildcard unittest/arch/$(ARCH)/.),)
$(error $(MODNAME) This module is not $(ARCH) compatible)
else
C_SRCS += $(shell find unittest/arch/$(ARCH)/ -name  '*.c')
ASM_SRCS += $(shell find unittest/arch/$(ARCH)/ -name '*.asm')
endif
endif
#todo: compiler call for generating test programm does not work yet
testmod: $(TEST_C_OBJS) $(TEST_ASM_OBJS)
	gcc -m32 -o test $^
%.o: %.c $(TEST_C_SRCS)
	$(CC) ${CFLAGS} $(L_CFLAGS) -c -o $@ $< $

%.o: %.asm $(TEST_ASM_SRCS)
	$(ASM) $(ASFLAGS) -o $@ $< $

run_test: testmod
	./testmod

clean:
	rm $(C_OBJS) $(ASM_OBJS) $(TEST_C_OBJS) $(TEST_ASM_OBJS) -f
	rm $(MODFILE) testmod -f
