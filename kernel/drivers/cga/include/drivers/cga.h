#ifndef _cga_h_
#define _cga_h_
/*
     Copyright 2012-2015 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *	@author Michael Sippel <micha@infinitycoding.de>
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

typedef struct __attribute__((packed)) cga_color
{
    int text_color : 4;
    int back_color : 3;
    int blink : 1;
} cga_color_t;

typedef struct __attribute__((packed)) cga_data
{
    uint8_t id;
    cga_color_t color;
} cga_data_t;

void INIT_CGA(void);

void cga_set_cursor(int x, int y);
void cga_set_color(cga_color_t color);
cga_color_t cga_color(int text, int back, int blink);

void cga_scroll(int lines);
void cga_clear_color(cga_color_t color);
void cga_clear(void);

void cga_putc_color(char c, cga_color_t col);
void cga_puts_color(const char *string, cga_color_t col);
void cga_putc(char c);
void cga_puts(const char *string);

#endif

