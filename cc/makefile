SHELL = /bin/bash

all: download configure_binutils binutils configure_gcc gcc

BINUTILS = binutils-2.24
GCC = gcc-4.7.3
GNU = ftp://ftp.gnu.org/gnu

BUILD = build
TARGET = target

download:
	$(MAKE) -C ../download load unpack NAME=$(BINUTILS) DPATH=$(GNU)/binutils/$(BINUTILS).tar.bz2 TYPE=tar.bz2
	$(MAKE) -C ../download load unpack NAME=$(GCC)      DPATH=$(GNU)/gcc/$(GCC)/$(GCC).tar.bz2    TYPE=tar.bz2
	cp -ru ../download/$(BINUTILS) ./$(BINUTILS)
	cp -ru ../download/$(GCC) ./$(GCC)
	mkdir -p $(BUILD)
	mkdir -p $(TARGET)

configure_binutils: download	
	if [ ! -f $(BINUTILS)/config.log ]; then \
		cp -rT binutils_adapt $(BINUTILS); \
		mkdir -p $(BUILD)/$(BINUTILS); \
		cd $(BUILD)/$(BINUTILS); \
		../../$(BINUTILS)/configure --prefix="`pwd`/../../target" --with-sysroot=`pwd` --target=i686-elf -disable-nls  CFLAGS="-w"; \
		cd ../..; \
	fi;\

binutils: configure_binutils
	$(MAKE) -C $(BUILD)/$(BINUTILS)
	$(MAKE) -C $(BUILD)/$(BINUTILS) install

configure_gcc: download binutils
	if [ ! -f $(GCC)/config.log ]; then \
		PATH=${PATH}:`pwd`/../../$(TARGET)/bin; \
		mkdir -p $(BUILD)/$(GCC); \
		cd $(BUILD)/$(GCC) ;\
		../../$(GCC)/configure --prefix="`pwd`/../../target" --target=i686-elf --disable-nls --enable-languages=c --disable-shared --disable-libssp --without-docdir CFLAGS="-I/usr/include/"; \
		cd ../..; \
	fi;\

gcc: configure_gcc
	$(MAKE) -C $(BUILD)/$(GCC)
	$(MAKE) -C $(BUILD)/$(GCC) install

clean:
	rm -rf $(BUILD)
	rm -rf $(TARGET)
	rm -rf $(BINUTILS)
	rm -rf $(GCC)

