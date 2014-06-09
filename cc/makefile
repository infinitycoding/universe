SHELL = /bin/bash

all: download configure_binutils binutils configure_gcc gcc

BINUTILS = binutils-2.24
GCC = gcc-4.7.3
GNU = ftp://ftp.gnu.org/gnu

BUILD = build
HOST_PREFIX = /usr/

download:
	$(MAKE) -C ../download load unpack NAME=$(BINUTILS) DPATH=$(GNU)/binutils/$(BINUTILS).tar.bz2 TYPE=tar.bz2
	$(MAKE) -C ../download load unpack NAME=$(GCC)      DPATH=$(GNU)/gcc/$(GCC)/$(GCC).tar.bz2    TYPE=tar.bz2
	cp -ru ../download/$(BINUTILS) ./$(BINUTILS)
	cp -ru ../download/$(GCC) ./$(GCC)
	mkdir -p $(BUILD)

configure_binutils: download	
	if [ ! -f $(BUILD)/$(BINUTILS)/config.log ]; then \
		cd $(BINUTILS); \
		patch -p1 < ../binutils.patch; \
		cd ..; \
		cp -rT binutils_adapted $(BINUTILS); \
		mkdir -p $(BUILD)/$(BINUTILS); \
		cd $(BUILD)/$(BINUTILS); \
		../../$(BINUTILS)/configure --prefix=$(HOST_PREFIX) --target=i686-universe -disable-nls  CFLAGS="-w"; \
		cd ../..; \
	fi;\

binutils: configure_binutils
	$(MAKE) -C $(BUILD)/$(BINUTILS)

configure_gcc: download binutils
	if [ ! -f $(BUILD)/$(GCC)/config.log ]; then \
		cd $(GCC); \
		patch -p1 < ../gcc.patch; \
		cd ..; \
		cp -rT gcc_adapted $(GCC); \
		PATH=${PATH}:`pwd`/../../$(TARGET)/bin; \
		mkdir -p $(BUILD)/$(GCC); \
		cd $(BUILD)/$(GCC) ;\
		../../$(GCC)/configure --prefix=$(HOST_PREFIX) --target=i686-universe --disable-nls --enable-languages=c --disable-shared --disable-libssp --without-docdir CFLAGS="-I/usr/include/"; \
		cd ../..; \
	fi;\

gcc: configure_gcc
	$(MAKE) -C $(BUILD)/$(GCC) all-gcc
#	$(MAKE) -C $(BUILD)/$(GCC) all-target-libgcc

install:
	$(MAKE) -C $(BUILD)/$(BINUTILS) install
	$(MAKE) -C $(BUILD)/$(GCC) install-gcc
#	$(MAKE) -C $(BUILD)/$(GCC) install-target-libgcc
	ln -s /usr/bin/i686-universe-gcc /usr/bin/i686-universe-cc

clean:
	rm -rf $(BUILD)
	rm -rf $(BINUTILS)
	rm -rf $(GCC)

