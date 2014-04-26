SHELL = /bin/bash

all: download configure build

KERNEL_HEADERS = `pwd`/../../../include

download:
	@ if [ ! -d glibc ]; then \
		git clone git://sourceware.org/git/glibc.git; \
	fi

configure: download
	@ mkdir -p build; \
	cd build; \
	if [ ! -f config.log ]; then \
		../glibc/configure --prefix=$(PWD) --without-misc --with-headers=$(KERNEL_HEADERS) --host=i686-unknown-linux-gnu --build=i686-unknown-linux-gnu CC="gcc -m32" CXX="g++ -m32" CFLAGS="-O2 -march=i686" CXXFLAGS="-O2 -march=i686";\
	fi

build: configure
	$(MAKE) -C build

clean:
	rm build/config.log
	$(MAKE) -C build clean

