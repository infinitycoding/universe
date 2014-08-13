all:
	mkdir -p ../build/usr/lib
	mkdir -p ../build/usr/include
	@$(MAKE) -C mercury
	@$(MAKE) -C udrcp
	#@$(MAKE) -C cdi
	#@$(MAKE) -C pfp

clean:
	@$(MAKE) -C mercury clean
	@$(MAKE) -C udrcp clean
	#@$(MAKE) -C cdi clean
	@$(MAKE) -C pfp clean

