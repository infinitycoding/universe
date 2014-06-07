SHELL = /bin/bash

all: download configure build

KERNEL_HEADERS = `pwd`/../../../kernel/include

SOURCEWARE = ftp://sourceware.org
NEWLIB = newlib-2.1.0

BUILD = build
TARGET = target

download:
	$(MAKE) -C ../../download load unpack NAME=$(NEWLIB) DPATH=$(SOURCEWARE)/pub/newlib/$(NEWLIB).tar.gz TYPE=tar.gz
	cp -ru ../../download/$(NEWLIB) ./$(NEWLIB)
	mkdir -p $(BUILD)
	mkdir -p $(TARGET)

configure: download
	if [ ! -f $(NEWLIB)/config.log ]; then \
		cd $(NEWLIB); \
		patch -p1 < ../newlib.patch; \
		mkdir -p newlib/libc/sys/universe; \
		cp -r ../system/* newlib/libc/sys/universe; \
		cd newlib/libc/sys/; \
		autoconf; \
		cd universe; \
		autoreconf; \
		cd ../../../../..; \
		cd $(BUILD); \
		../$(NEWLIB)/configure --prefix="`pwd`/../target" --target=i686-universe; \
		cd ..; \
	fi

build: configure
	$(MAKE) -C $(BUILD)

clean:
	rm -rf $(NEWLIB)
	rm -rf $(BUILD)
	rm -rf $(TARGET)

