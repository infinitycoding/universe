all:
	mkdir -p ../build/usr/lib
	mkdir -p ../build/usr/include
	@$(MAKE) -C mercury TARGET="universe" CCPP="$(CCPP)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)" ARCH="$(ARCH)"
	@$(MAKE) -C udrcp ARCH="$(ARCH)"
	#@$(MAKE) -C cdi

clean:
	@$(MAKE) -C mercury clean TARGET="universe" ARCH="$(ARCH)"
	@$(MAKE) -C udrcp clean
	@$(MAKE) -C cdi clean

