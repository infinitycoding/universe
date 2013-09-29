
all:
	nasm -f elf32 -o entry.o entry.asm
	$(MAKE) -C test

clean:
	$(MAKE) -C test clean
	rm entry.o


