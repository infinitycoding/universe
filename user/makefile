
all:
	@nasm -f elf32 -o entry.o entry.asm
	@$(MAKE) -C test
	@$(MAKE) -C ultrashell

clean:
	@$(MAKE) -C test clean
	@$(MAKE) -C ultrashell clean
	@rm entry.o -f


