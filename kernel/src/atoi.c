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



	 Diese Datei ist ein Teil vom Universe Kernel.

	 Das Universe Kernel ist Freie Software: Sie koennen es unter den Bedingungen
	 der GNU General Public License, wie von der Free Software Foundation,
	 Version 3 der Lizenz oder jeder sp‰teren
	 veroeffentlichten Version, weiterverbreiten und/oder modifizieren.

	 Das Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
	 Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
	 OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
	 Gew‰hrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN ZWECK.
	 Siehe die GNU General Public License fuer weitere Details.

	 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
	 Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

/**
	@author Tom Slawik <tom.slawik@gmail.com>
*/

#include <atoi.h>
/*
	TODO: Make it compatible with both unsigned int and signed int
*/
char * itoa_ex(unsigned int value, char * str, int base, int flags, int width)
{
	char *result = str;
	int size;
	
	int len = 0;
	int temp = 0;
	int negative = 0;

	const char *digits = "0123456789ABCDEF";

	if (flags & SMALL) {
		digits = "0123456789abcdef";
	}

	if (value < 0) {
		value = - value;
		++negative;
	}

	temp = value;
	do {temp /= base; ++len;} while (temp);
	size = len;

	if (!(flags & LEFT) && !(flags & ZEROPAD))
		while (size < width--)
			*str++ = ' ';

	if (negative) {
		*str++ = '-';
	} else if (flags & PLUS) {
		*str++ = '+';
	}
	if (flags & SPECIAL) {
		*str++ = '0'; *str++ = 'x';
		width -= 2;
	}
	
	if (!(flags & LEFT) && (flags & ZEROPAD))
		while (size < width--)
			*str++ = '0';

	do {
		int power = powi(base, --len);
		int digit = value / power;
		
		*str++ = digits[digit];
		
		value -= digit * power;
	} while (len > 0);

	while (size < width--)
		*str++ = ' ';
	
	*str++ = '\0';

	return result;
}

inline char * itoa(unsigned int value, char * str, int base)
{
	return itoa_ex(value, str, base, 0, 1);
}

int atoi(const char *str)
{
	int result = 0;
	int length = 0;

	while (isdigit(*str++))
		++length;

	str -= length + 1;

	while (length > 0)
		result += powi(10, --length) * (*str++ - '0');

	return result;
}
