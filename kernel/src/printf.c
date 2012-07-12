/* unclean development file - be aware */

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "printf.h"

#define NULL 0

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

int sprintf_test(char * str, const char * format, ...)
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
			
			for (i = 0; i < ftag.width - k; ++i)
				buf[len++] = ftag.flags;

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

	/* flags */
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

	/* width */
	if (*format == '*') {
		ftag->width = -1;
	} else if (isdigit(*format)) {
		ftag->width = atoi(format);
		while (isdigit(*++format));
	}

	/* precision */
	if (*format == '.') {
		++format;
		if (*format == '*') {
			ftag->precision = -1;
		} else if (isdigit(*format)) {
			ftag->precision = atoi(format);
			while (isdigit(*++format));
		}
	}

	/* length */
	switch (*format) {
		case 'h':
		case 'l':
		case 'L':
			ftag->length = *format;
			++format;
	}

	/* specifier */
	ftag->specifier = *format;

	return format;
}
