all:
	@$(MAKE) -C libc
	@$(MAKE) -C glibc configure
	@$(MAKE) -C udrcp
	@$(MAKE) -C crt
	@$(MAKE) -C cdi

clean:
	@$(MAKE) -C libc clean
	@$(MAKE) -C udrcp clean
	@$(MAKE) -C crt clean
	@$(MAKE) -C cdi clean

