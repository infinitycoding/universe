SHELL = /bin/bash

all: download configure_binutils binutils configure_gcc gcc

BINUTILS = binutils-2.24
GCC = gcc-4.7.3

download:
	@ if [ ! -d $(BINUTILS) ]; then \
		wget ftp.gnu.org/gnu/binutils/$(BINUTILS).tar.bz2; \
		tar -xjf $(BINUTILS).tar.bz2; \
	fi
	@ if [ ! -d $(GCC) ]; then \
		wget ftp.gnu.org/gnu/gcc/$(GCC)/$(GCC).tar.bz2; \
		tar -xjf $(GCC).tar.bz2; \
	fi
	mkdir -p target
	mkdir -p build

configure_binutils: download
	mkdir -p build/$(BINUTILS); \
	cd build/$(BINUTILS); \
	if [ ! -f config.log ]; then \
		../../$(BINUTILS)/configure --prefix="`pwd`/../../target" --with-sysroot=`pwd` --target=i686-elf -disable-nls  CFLAGS="-w"; \
	fi;\
	cd ../..;

binutils: configure_binutils
	$(MAKE) -C build/$(BINUTILS)
	$(MAKE) -C build/$(BINUTILS) install

configure_gcc: download binutils
	export PATH = $PATH:`pwd`/../../target/bin; \
	mkdir -p build/$(GCC); \
	cd build/$(GCC) ;\
	if [ ! -f config.log ]; then \
		../../$(GCC)/configure --prefix="`pwd`/../../target" --target=i686-elf --disable-nls --enable-languages=c --disable-shared --disable-libssp --without-docdir CFLAGS="-I/usr/include/"; \
	fi;\
	cd ../..;

gcc: configure_gcc
	$(MAKE) -C build/$(GCC)
	$(MAKE) -C build/$(GCC) install

clean:
	rm build/$(BINUTILS)/config.log
	rm build/$(GCC)/config.log
	$(MAKE) -C build/$(BINUTILS) clean
	$(MAKE) -C build/$(GCC) clean

