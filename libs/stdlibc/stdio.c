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
 *@author Simon Diepold aka. tdotu <simon.diepold@infinitycoding.de>
 **/

#include <universe.h>
#include <stdio.h>

int write(int fd,void *buffer, size_t n)
{
    return linux_syscall(SYS_WRITE, fd, (uint32_t)buffer, n, 0, 0);
}

int read(int fd, void *buffer,size_t n)
{
    return linux_syscall(SYS_READ, fd, (uint32_t)buffer, n, 0, 0);
}

int open(const char *path, int file_access, int filemode)
{
    return linux_syscall(SYS_OPEN, (uint32_t)path, file_access, filemode, 0, 0);
}

int close(int fd)
{
    return linux_syscall(SYS_CLOSE, fd, 0, 0, 0, 0);
}

int getchar(void)
{
	char c;
        read(stdin, &c, 1);
    	return (int)c;
}
