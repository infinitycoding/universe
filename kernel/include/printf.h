#ifndef _printf_h_
#define _printf_h_
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

#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

typedef struct {
		char flags;
		int width;
		int precision;
		char length;
		char specifier;
} ftag_t;

char * itoa(int value, char * str, int base);
int atoi(const char *str);

int vsprintf(char * str, const char * format, va_list arg);
int sprintf(char * str, const char * format, ...);

int ftag_format(char *buf, int len, void *obj, ftag_t ftag);
const char * ftag_scan(ftag_t *ftag, const char *format);

void clear_screen(void);
int printf(const char * format, ...);

#define DEFAULT_FRONT_COLOR	GREEN
#define DEFAULT_BACK_COLOR	BLACK

#endif
