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

#include <drivers/crtc.h>
#include <drivers/video.h>
#include <io.h>

static int lines = 25;
static int columns = 80;

static int x = 0;
static int y = 0;

static color_t color = CYAN | BLACK << 4;
static char *video_mem = (char *)0xc00b8000;

int putchar(int c)
{
	if (c == '\b') {
		if (x > 0) {
			gotoxy(--x, y);
		} else {
			gotoxy(columns - 1, --y);
		}
		
		video_mem[2 * (y * columns + x)] = 0;
		
		return c;
	}
	
	if ((x > columns - 1) || (c == '\n')) {
		gotoxy(0, ++y);

		if (c == '\n') {
			return c;
		}
	}

	video_mem[2 * (y * columns + x)] = c;
	video_mem[2 * (y * columns + x) + 1] = color;

	gotoxy(++x, y);
	
	return c;
}

int puts(const char* s)
{
	int printed = 1;

	while (*s) {
		putchar(*s++);
		++printed;
	}
	putchar('\n');

	return printed;
}

int fputs(const char* s, int fd)
{
	if (fd == STDOUT) {
		while (*s) {
			putchar(*s++);
		}
	}

	return 0;
}

void clear_screen(void)
{
	int i;
	for (i = 0; i < lines * columns; i++) {
		video_mem[2 * i] = 0;
		video_mem[2 * i + 1] = color;
	}

	gotoxy(0, 0);
}

void set_color(color_t _color)
{
	color = _color;
}

color_t get_color(void)
{
	return color;
}

void gotoxy(uint8_t _x, uint8_t _y)
{
	uint16_t offset;

	if ((_y * columns + _x) >= (columns * lines)) { /* scroll if neccessary */
		memmove(video_mem, video_mem + 2 * columns, 2 * lines * columns);
		memset(video_mem + 2 * lines * columns, 0, 2 * columns);
		--_y;
	}
	
	x = _x; y = _y;
	offset = _y * columns + _x;

	if (video_mem[2 * offset] == 0) {
		video_mem[2 * offset + 1] = color;
	}

	crtc_write(CRTC_CURSOR_LOCATION_HIGH, (uint8_t)(offset >> 8));
	crtc_write(CRTC_CURSOR_LOCATION_LOW, (uint8_t)offset);
}
