SHELL = /bin/bash

all: download configure build

KERNEL_HEADERS = `pwd`/../../../kernel/include/linux-meta

download:
	@ if [ ! -d gcc-4.9.0 ]; then \
		wget http://www.mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-4.9.0/gcc-4.9.0.tar.bz2; \
		tar -xjf gcc-4.9.0.tar.bz2; \
	fi

configure: download
	@ mkdir -p build; \
	cd build; \
	if [ ! -f config.log ]; then \
		../gcc-4.9.0/configure --prefix=$(PWD) --target=i386-elf --disable-nls --enable-languages=c --program-prefix=universe --disable-shared --disable-libssp --without-headers; \
	fi

build: configure
	$(MAKE) -C build

clean:
	rm build/config.log
	$(MAKE) -C build clean

