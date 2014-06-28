SHELL = /bin/bash

all: download configure build install

SOURCEWARE = ftp://sourceware.org
NEWLIB = newlib-2.1.0

BUILD = build


download:
	$(MAKE) -C ../../download load unpack NAME=$(NEWLIB) DPATH=$(SOURCEWARE)/pub/newlib/$(NEWLIB).tar.gz TYPE=tar.gz
	cp -ru ../../download/$(NEWLIB) ./$(NEWLIB)
	mkdir -p $(BUILD)

configure: download
	if [ ! -f $(BUILD)/config.log ]; then \
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
		../$(NEWLIB)/configure --prefix=$(PREFIX)/usr --target=$(ARCH)-universe; \
		cd ..; \
	fi

build: configure
	$(MAKE) -C $(BUILD)

install: build
	$(MAKE) -C $(BUILD) install
	@rm -R $(PREFIX)/usr/share
	@cp -R $(PREFIX)/usr/$(ARCH)-universe/* $(PREFIX)/usr/
	@rm -rf $(PREFIX)/usr/$(ARCH)-universe

clean:
	rm -rf $(NEWLIB)
	rm -rf $(BUILD)
