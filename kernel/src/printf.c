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

    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <printf.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <atoi.h>
#include <video.h>

int printf(const char *fmt, ...)
{
	int size;
	char buffer[1024];
	memset(buffer, 0, 1024);

	va_list args;
	va_start(args, fmt);

	size = vsprintf(buffer, fmt, args);

	puts(buffer);

	va_end(args);
	return size;
}

int sprintf(char *buf, const char *fmt, ...)
{
	int size;

	va_list args;
	va_start(args, fmt);

	size = vsprintf(buf, fmt, args);

	va_end(args);
	return size;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *str = buf;
	const char *s;
	int i, len, num;

	int flags;
	int width;
	int precision;
	int qualifier;

	int base;

	for (; *fmt; ++fmt) {
		if (*fmt != '%') {
			*buf++ = *fmt;
			continue;
		}

		/* flags */
		flags = 0;
		loop:
		++fmt;
		switch (*fmt) {
		case '0':
			flags |= ZEROPAD;
			goto loop;
		case '+':
			flags |= PLUS;
			goto loop;
		case ' ':
			flags |= SPACE;
			goto loop;
		case '-':
			flags |= LEFT;
			goto loop;
		case '#':
			flags |= SPECIAL;
			goto loop;
		}

		/* width */
		width = 1;
		if (isdigit(*fmt)) {
			width = atoi(fmt);
			while (isdigit(*++fmt));
		} else if (*fmt == '*') {
			++fmt;
			width = va_arg(args, int);
			if (width < 0) {
				width = - width;
				flags |= LEFT;
			}
		}

		/* precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt)) {
				precision = atoi(fmt);
				while (isdigit(*++fmt));
			} else if (*fmt == '*') {
				++fmt;
				precision = va_arg(args, int);
			}
			if (width < 0) {
				width = 0;
			}
		}

		/* qualifier */
		qualifier = -1;
		if (strchr("hlL", *fmt)) {
			qualifier = *fmt;
			++fmt;
		}

		/* base */
		base = 10;

		/* specifier */
		switch (*fmt) {
		case 'c':
			len = 1;

			if (!(flags & LEFT))
				while (len < width--)
					*buf++ = ' ';

			*buf++ = (unsigned char)va_arg(args, int);

			while (len < width--)
				*buf++ = ' ';
			continue;

		case 's':
			s = va_arg(args, char *);
			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < width--)
					*buf++ = ' ';

			for (i = 0; i < len; ++i)
				*buf++ = *s++;

			while (len < width--)
				*buf++ = ' ';
			continue;

		case 'p':
			if (width == -1) {
				width = 2 * sizeof(void *);
				flags |= ZEROPAD;
			}
			itoa_ex((unsigned long)va_arg(args, void *), buf, 16, flags, width);
			continue;

		case 'n':
			if (qualifier == 'l') {
				long *ip = va_arg(args, long *);
				*ip = (buf - str);
			} else {
				int *ip = va_arg(args, int *);
				*ip = (buf - str);
			}
			continue;

		case '%':
			*buf++ = '%';

		/* integers */

		case 'o':
			base = 8;
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}

		if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
		} else if (qualifier == 'h') {
			num = (unsigned short)va_arg(args, int);
			if (flags & SIGN) {
				num = (short)num;
			}
		} else if (flags & SIGN) {
			num = va_arg(args, int);
		} else {
			num = va_arg(args, unsigned int);
		}

		itoa_ex(num, buf, base, flags, width);
		while (*++buf);
	}

	*buf = '\0';

	return buf - str;
}
