SHELL = /bin/bash

load:
	if [ ! -f ${NAME}.${TYPE} ]; then \
		wget ${DPATH}; \
	fi

unpack: load
	if [ ! -d ${NAME} ]; then \
		if [ "${TYPE}" == "tar.gz" ]; then \
			tar -xzf ${NAME}.${TYPE}; \
		else \
			tar -xjf ${NAME}.${TYPE}; \
		fi; \
	fi

