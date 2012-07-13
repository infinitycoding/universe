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
#include <printf.h>

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
    video_mem[2 * (y * 80 + x) + 1] = 0x07;

    x++;
}

void puts(const char* s)
{
    while (*s) {
        putc(*s++);
    }
}

void clear(void)
{
    int i;
    for (i = 0; i < 2 * 25 * 80; i++) {
        video_mem[i] = 0;
    }

    x = y = 0;
}

int printf(const char *param, ...)
{
    va_list ap;
    const char* s;
    unsigned long n;

    va_start(ap, param);
    while (*param) {
        if (*param == '%') {
            param++;
            switch (*param) {
                case 's':
                    s = va_arg(ap, char *);
                    puts(s);
                    break;
                case 'c':
                    n = va_arg(ap, int);
                    putc(n);
                    break;
                case '%':
                    putc('%');
                    break;
                case '\0':
                    va_end(ap);
                    return 0;
                default:
                    putc('%');
                    putc(*param);
                    break;
            }
        } else {
            putc(*param);
        }

        param++;
    }

    va_end(ap);

    return 0;
}
