
all:
	@nasm -f elf32 -o entry.o entry.asm
	@$(MAKE) -C keyboard

clean:
	@$(MAKE) -C keyboard clean
	@rm entry.o -f


