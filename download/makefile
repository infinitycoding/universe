SHELL = /bin/bash

load:
	if [ ! -e ${NAME}.tar.bz2 ]; then \
		wget ${DPATH}; \
	fi

unpack: load
	if [ ! -d ${NAME} ]; then \
		tar -xjf ${NAME}.tar.bz2; \
	fi

