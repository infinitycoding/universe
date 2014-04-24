SHELL = /bin/bash

all: download configure build

KERNEL_HEADERS = ../../../kernel/include

download:
	@ if [ ! -d glibc ]; then \
		git clone git://sourceware.org/git/glibc.git; \
	fi

configure: download
	@ mkdir -p build; \
	cd build; \
	if [ ! -f config.log ]; then \
		../glibc/configure --prefix=$(PWD) --with-headers=$(KERNEL_HEADERS); \
	fi

build: configure
	$(MAKE) -C build

clean:
	rm build/config.log
	$(MAKE) -C build clean

