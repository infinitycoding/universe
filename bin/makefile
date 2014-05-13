SHELL = /bin/bash

all: download configure_binutils binutils configure_gcc gcc

BINUTILS = binutils-2.24
GCC = gcc-4.7.3
GNU = ftp://ftp.gnu.org/gnu

download:
	$(MAKE) -C ../download load unpack NAME=$(BINUTILS) DPATH=$(GNU)/binutils/$(BINUTILS).tar.bz2
	$(MAKE) -C ../download load unpack NAME=$(GCC)      DPATH=$(GNU)/gcc/$(GCC)/$(GCC).tar.bz2
	cp -ru ../download/$(BINUTILS) ./$(BINUTILS)
	cp -ru ../download/$(GCC) ./$(GCC)
	mkdir -p target
	mkdir -p build

configure_binutils: download	
	cd $(BINUTILS); \
	patch -p1 < ../binutils.patch; \
	cd ..; \
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
	PATH = ${PATH}:`pwd`/../../target/bin; \
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

