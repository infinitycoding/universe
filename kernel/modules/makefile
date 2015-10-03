SUBDIRS = $(wildcard */)

ifeq (${ARCH}, i686)
IPATHS = -I../../../arch/x86/include
else ifeq (${ARCH}, arm)
IPATHS = -I../../../arch/arm/include
endif

ifeq (${PLATFORM},pc)
IPATHS += -I../../../platform/pc/include
endif

all:
	@for mod in $(MODULES); do \
		$(MAKE) -C $$mod build ARCH="$(ARCH)" PLATFORM="$(PLATFORM)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)" CFLAGS="$(CFLAGS) $(IPATHS) `../getDependencies.sh . $$mod`" ASFLAGS="$(ASFLAGS)" LDFLAGS="$(LDFLAGS)";\
	done
	#for mod in $(MODULES); do \
	#	$(MAKE) -C $$mod test ARCH="$(ARCH)" PLATFORM="$(PLATFORM)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)" CFLAGS="$(CFLAGS) $(IPATHS)" ASFLAGS="$(ASFLAGS)" LDFLAGS="$(LDFLAGS)"; \
	#done

clean:
	for mod in $(SUBDIRS); do \
		$(MAKE) -C $$mod clean; \
	done
