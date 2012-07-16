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

/*
	text output
*/

static int x = 0;
static int y = 0;

static char *video_mem = (char *)0xb8000;

void putc(char c)
{
    if((x > 79) || (c == '\n')) {
        x = 0;
        y++;
        if (c == '\n') {
            return;
        }
    }

    video_mem[2 * (y * 80 + x)] = c;
    video_mem[2 * (y * 80 + x) + 1] = DEFAULT_FRONT_COLOR | DEFAULT_BACK_COLOR;
    x++;
    
    if( (y * 80 + x) > (80 * 25) ){
      scroll();
    }
}

void puts(const char* s)
{
    while (*s) {
        putc(*s++);
    }
}

void clear_screen(void)
{
    int i;
    for (i = 0; i < 2 * 25 * 80; i++) {
        video_mem[i] = 0;
    }

    x = y = 0;
}

void scroll(void)
{
    int i;
    y --;
    for(i = 0; i < 3840; i++)
      video_mem[i] = video_mem[i + 80];
}

/*
	text formatting
*/

char * itoa(int value, char * str, int base)
{
	int i = 0;
	int length;

	if (value < 0) {
		value = abs(value);
		str[i++] = '-';
	}

	length = log2(value) / log2(base) + 1;

	while (length > 0) {
		int power = pow(base, --length);
		int digit = value / power;
		if (digit < 10) {
			str[i++] = '0' + digit;
		} else {
			str[i++] = 'A' + digit - 10;
		}
		value -= digit * power;
	}

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

int printf(const char * format, ...)
{
	char buffer[1024]; /* TODO: dynamic buffer size */

	va_list arg;
	va_start(arg, format);

	vsprintf(buffer, format, arg);

	puts(buffer);

	va_end(arg);
}

int sprintf(char * str, const char * format, ...)
{
	va_list arg;
	va_start(arg, format);

	return vsprintf(str, format, arg);

	va_end(arg);
}

int vsprintf(char * str, const char * format, va_list arg)
{
	ftag_t ftag;
	int len = 0;

	while (*format != NULL)
	{
		if (*format == '%') {
			format = ftag_scan(&ftag, ++format);
			len = ftag_format(str, len, va_arg(arg, void *), ftag);
		} else {
			str[len++] = *format;
		}

		++format;
	}

	return len;
}

int ftag_format(char *buf, int len, void *obj, ftag_t ftag)
{
	int base = 10;
	int i, k;
	char buffer[10];
	char *ptrchr;


	switch (ftag.specifier)
	{
		case 'c':
			for (i = 0; i < ftag.width - 1; ++i)
			buf[len++] = ftag.flags;
			buf[len++] = (char)obj;
		break;
		case 'd': case 'i': case 'u': case 'o': case 'x': case 'X':
			if (ftag.specifier == 'o') {
				base = 8;
			} else if (ftag.specifier == 'x' || ftag.specifier == 'X') {
				base = 16;
			} else {
				base = 10;
			}
	
			i = (int)obj;
			itoa(i, buffer, base);
		
			k = strlen(buffer);
	
			if (ftag.specifier == 'x') {
				for (i = 0; i < k; ++i) {
					buffer[i] = tolower(buffer[i]);
				}
			}
			
			if (ftag.flags == '#' && base == 16) {
				buf[len++] = '0';
				buf[len++] = 'x';
			} else {
				for (i = 0; i < ftag.width - k; ++i) {
					buf[len++] = ftag.flags;
				}
			}
	
			strncpy(buf + len, buffer, k);
			len += k;
		break;
		case 's':
			k = strlen((char *)obj);
			for (i = 0; i < ftag.width - 1; ++i)
			buf[len++] = ftag.flags;
			strncpy(buf + len, (char *)obj, k);
			len += k;
		break;
		case '%':
			for (i = 0; i < ftag.width - 1; ++i)
				buf[len++] = ftag.flags;
			buf[len++] = '%';
	}

	return len;
}

const char * ftag_scan(ftag_t *ftag, const char *format)
{
	ftag->flags = ' ';
	ftag->width = 0;
	ftag->precision = 1;
	ftag->length = 'l';
	ftag->specifier = '\0';

	// flags
	switch (*format)
	{
		case '-':
		case '+':
		case ' ':
		case '#':
		case '0':
			ftag->flags = *format;
			++format;
	}

	// width
	if (*format == '*') {
		ftag->width = -1;
	} else if (isdigit(*format)) {
		ftag->width = atoi(format);
		while (isdigit(*++format));
	}

	// precision
	if (*format == '.') {
		++format;
		if (*format == '*') {
			ftag->precision = -1;
		} else if (isdigit(*format)) {
			ftag->precision = atoi(format);
			while (isdigit(*++format));
		}
	}

	// length
	switch (*format) {
		case 'h':
		case 'l':
		case 'L':
		ftag->length = *format;
		++format;
	}

	// specifier
	ftag->specifier = *format;

	return format;
}

