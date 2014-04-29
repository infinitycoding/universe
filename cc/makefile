SHELL = /bin/bash

all: download configure build

KERNEL_HEADERS = `pwd`/../../../kernel/include/linux-meta

download:
	@ if [ ! -d gcc-4.4.2 ]; then \
		wget http://ftp.gnu.org/gnu/gcc/gcc-4.4.2/gcc-core-4.4.2.tar.bz2; \
		tar -xjf gcc-core-4.4.2.tar.bz2; \
	fi

configure: download
	@ mkdir -p build; \
	cd build; \
	if [ ! -f config.log ]; then \
		../gcc-4.4.2/configure --prefix=$(PWD) --target=i386-elf --disable-nls --enable-languages=c --disable-shared --disable-libssp --without-headers; \
	fi

build: configure
	$(MAKE) -C build

clean:
	rm build/config.log
	$(MAKE) -C build clean

