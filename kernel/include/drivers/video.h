#ifndef _video_h_
#define _video_h_
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

#define BLACK		0x0
#define BLUE		0x1
#define GREEN		0x2
#define CYAN		0x3
#define RED 		0x4
#define MAGENTA		0x5
#define BROWN		0x6
#define LIGHT_GRAY	0x7
#define DARK_GRAY	0x8
#define LIGHT_BLUE	0x9
#define LIGHT_GREEN	0xA
#define LIGHT_CYAN	0xB
#define LIGHT_RED	0xC
#define LIGHT_MAGENTA	0xD
#define YELLOW		0xE
#define WHITE		0xF

typedef uint8_t color_t;

#define STDOUT 1

int putchar(int c);
int puts(const char* s);
int fputs(const char* s, int fd);
void clear_screen(void);
void set_color(color_t _color);
color_t get_color(void);
void gotoxy(uint8_t _x, uint8_t _y);

#endif
