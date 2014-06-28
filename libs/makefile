all:
	mkdir -p ../build/usr/lib
	mkdir -p ../build/usr/include
	@$(MAKE) -C libc
	@$(MAKE) -C udrcp
	@$(MAKE) -C crt
	#@$(MAKE) -C cdi
	@$(MAKE) -C pfp

clean:
	@$(MAKE) -C libc clean
	@$(MAKE) -C udrcp clean
	@$(MAKE) -C crt clean
	@$(MAKE) -C newlib clean
	#@$(MAKE) -C cdi clean
	@$(MAKE) -C pfp clean

