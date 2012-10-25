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

#include <math.h>


int powi(int base, int exponent)
{
	int result = base;
	int i;

	if (exponent == 0) {
		result = 1;
	} else {
		for (i = 0; i < exponent - 1; ++i) {
			result *= base;
		}
	}

	return result;
}

int logi(int x)
{
	int log2 = 0;
	int pot = 1;
	while (pot <= x) {
		pot *= 2;
		++log2;
	}
	return log2 - 1;
}

int absi(int x)
{
	if (x < 0) {
		return - x;
	} else {
		return x;
	}
}

unsigned int bit_scan_forward(unsigned int i)
{
	unsigned int j;

	for (j = 0; j < 32; j++) {
		if (i & (1 << j)) {
			return j;
		}
	}
	return -1;
}
