#ifndef _CGA_H_
#define _CGA_H_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the universe CGA-Driver.
 
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
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

 #include <stdint.h>
 #include <crtc.h>


#define BLACK      0x0
#define BLUE        0x1
#define GREEN       0x2
#define CYAN        0x3
#define RED         0x4
#define MAGENTA     0x5
#define BROWN       0x6
#define LIGHT_GRAY  0x7
#define DARK_GRAY   0x8
#define LIGHT_BLUE  0x9
#define LIGHT_GREEN 0xA
#define LIGHT_CYAN  0xB
#define LIGHT_RED   0xC
#define LIGHT_MAGENTA    0xD
#define YELLOW      0xE
#define WHITE       0xF

typedef uint8_t color_t;

struct cga_handle
{
    int lines;
    int cols;
    int xpos;
    int ypos;
    unsigned char *video_mem;
};

void gotoxy(struct crtc_handle *crtc, struct cga_handle *cga, uint8_t _x, uint8_t _y, int color);
int putchar(struct crtc_handle *crtc, struct cga_handle *cga, int c, int color);

#endif