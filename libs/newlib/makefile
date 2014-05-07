SHELL = /bin/bash

all: download configure build

KERNEL_HEADERS = `pwd`/../../../kernel/include

download:
	@ if [ ! -d newlib-2.1.0 ]; then \
		wget ftp://sourceware.org/pub/newlib/newlib-2.1.0.tar.gz; \
		tar -xzvf newlib-2.1.0.tar.gz;\
		rm newlib-2.1.0.tar.gz;\
	fi

configure: download
	@ mkdir -p build; \
	@ mkdir -p target; \
	cd build; \
	if [ ! -f config.log ]; then \
		../newlib-2.1.0/configure --prefix='`pwd`/../target' --target=i686-elf;\
	fi

build: configure
	$(MAKE) -C build

clean:
	rm build/config.log
	rm -rf target/*
	$(MAKE) -C build clean

