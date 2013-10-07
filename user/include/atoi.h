#ifndef _atoi_h_
#define _atoi_h_

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
	@author Tom Slawik <tom.slawik@gmail.com>
*/

#include <stdint.h>

#define ZEROPAD 1
#define SIGN	2
#define PLUS	4
#define SPACE	8
#define LEFT	16
#define SMALL	32
#define SPECIAL	64

extern char * itoa_ex(unsigned int value, char * str, int base, int flags, int width);
extern inline char * itoa(unsigned int value, char * str, int base);
extern int atoi(const char *str);

#endif