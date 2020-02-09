all:
	mkdir -p ../build/usr/lib
	mkdir -p ../build/usr/include
	@$(MAKE) -C mercury TARGET="universe" CCPP="$(CCPP)" CC="$(CC)" ASM="$(ASM)" LD="$(LD)"
	@$(MAKE) -C udrcp
	#@$(MAKE) -C cdi

clean:
	@$(MAKE) -C mercury clean
	@$(MAKE) -C udrcp clean
	@$(MAKE) -C cdi clean

