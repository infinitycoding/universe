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

#include <atoi.h>

char * itoa_ex(int value, char * str, int base, int flags, int width)
{
	char *result = str;
	int len = 0;
	int temp = 0;
	int negative = 0;

	static const char *digits = "0123456789ABCDEF";

	if (flags & SMALL) {
		digits = "0123456789abcdef";
	}

	if (value < 0) {
		value = abs(value);
		++negative;
	}

	temp = value;
	do {temp /= base; ++len;} while (temp);

	if (!(flags & LEFT))
		while (len < width--)
			*str++ = ' ';

	if (negative) {
		*str++ = '-';
	} else if (flags & PLUS) {
		*str++ = '+';
	}
	if (flags & SPECIAL) {
		*str++ = '0';
		*str++ = 'x';
	}

	do {
		int power = pow(base, --len);
		int digit = value / power;
		
		*str++ = digits[digit];
		
		value -= digit * power;
	} while (len > 0);

	while (len < width--)
		*str++ = ' ';
	
	*str++ = '\0';

	return result;
}

char * itoa(int value, char * str, int base)
{
	static const char digits[16] = "0123456789ABCDEF";
	
	int i = 0;
	int length = 0;
	int temp = 0;

	if (value < 0) {
		value = abs(value);
		str[i++] = '-';
	}

	temp = value;
	do {temp /= base; ++length;} while (temp);

	do {
		int power = pow(base, --length);
		int digit = value / power;
		
		str[i++] = digits[digit];
		
		value -= digit * power;
	} while (length > 0);

	str[i++] = '\0';

	return str;
}

int atoi(const char *str)
{
	int result = 0;
	int length = 0;

	while (isdigit(*str++))
		++length;

	str -= length + 1;

	while (length > 0)
		result += pow(10.00f, --length) * (*str++ - '0');

	return result;
}
