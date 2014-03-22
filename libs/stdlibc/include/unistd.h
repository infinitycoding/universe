#ifndef _unistd_h_
#define _unistd_h_

/*
	Copyright 2012 universe coding group (UCG) all rights reserved
	This file is part of the Universe Kernel.

	Universe Kernel is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	Universe Kernel is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
	@author Michael Sippel <micha.linuxfreak@gmail.com>
*/
#include <stdint.h>

#define R_OK 0x1
#define W_OK 0x2
#define X_OK 0x4
#define F_OK 0x8

typedef struct dirent {
  int id;
  char name[256];
  stat_t stat;
} dirent_t;

int open(const char *path, int file_access, int filemode);
int close(int fd);
int read(int fd, void *buffer,size_t n);
int write(int fd,void *buffer, size_t n);
dirent_t *readdir(int fd);
int chdir(const char *path);

#endif
