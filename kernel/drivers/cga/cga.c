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
 *  @author Michael Sippel <micha@infinitycoding.de>
 */
#include <platform.h>
#ifdef _CGA_

#include <drivers/cga.h>
#include <io.h>

#include <string.h>

#define CRTC_INDEX_PORT 		0x3D4
#define CRTC_DATA_PORT 			0x3D5
#define CRTC_CURSOR_LOCATION_HIGH 	0x0E
#define CRTC_CURSOR_LOCATION_LOW 	0x0F

#define cga_size_x  80
#define cga_size_y  25
#define cga_size (cga_size_x * cga_size_y)

#define cga_off(x, y) (x + y * cga_size_x)

static cga_data_t *cga_mem;
static cga_color_t cga_default_color;
static int cga_x;
static int cga_y;

void INIT_CGA(void)
{
    cga_mem = (cga_data_t*) vmm_automap_kernel(current_context, 0xb8000, VMM_WRITABLE);
    cga_default_color.text_color = WHITE;
    cga_default_color.back_color = BLACK;
    cga_x = 0;
    cga_y = 0;
}

void cga_set_cursor(int x, int y)
{
    cga_x = x;
    cga_y = y;
    uint16_t offset = cga_off(x, y);

    outb(CRTC_INDEX_PORT, CRTC_CURSOR_LOCATION_HIGH);
    outb(CRTC_DATA_PORT, (uint8_t) (offset >> 8));

    outb(CRTC_INDEX_PORT, CRTC_CURSOR_LOCATION_LOW);
    outb(CRTC_DATA_PORT, (uint8_t) offset);
}

int cga_correct_coords(int *x, int *y)
{
    int line = *y + (*x / cga_size_x);
    *x = *x % cga_size_x;

#if 0
    *y = *y % cga_size_y;
    return 0;
#else
    if(line >= cga_size_y)
    {
        *x = 0;
        *y = cga_size_y - 1;
        return (line - cga_size_y) + 1;
    }
    else
    {
        *y = line;
        return 0;
    }
#endif
}

void cga_set_color(cga_color_t col)
{
    cga_default_color = col;
}

cga_color_t cga_color(int text, int back, int blink)
{
    cga_color_t color;
    color.text_color = text;
    color.back_color = back;
    color.blink = blink;

    return color;
}

void cga_scroll(int lines)
{
    if(lines >= cga_size_y)
    {
        cga_clear();
    }
    else if(lines > 0)
    {
        memcpy((void*) cga_mem, (void*) cga_mem + sizeof(cga_data_t)*cga_off(0, lines), sizeof(cga_data_t) * cga_off(0, (cga_size_y-lines)));
        memset((void*) cga_mem + sizeof(cga_data_t)*cga_off(0, (cga_size_y-lines)), 0, sizeof(cga_data_t) * cga_off(0, lines));
    }
}

void cga_clear_color(cga_color_t color)
{
    int i;
    for(i = 0; i < cga_size; i++)
    {
        cga_mem[i].id = 0;
        cga_mem[i].color = color;
    }

    cga_set_cursor(0, 0);
}

void cga_clear(void)
{
    cga_clear_color(cga_default_color);
}

void cga_putc_color(char c,	cga_color_t color)
{
    uint16_t offset = cga_off(cga_x, cga_y);
    cga_mem[offset].id = c;
    cga_mem[offset].color = color;

    cga_x ++;
    cga_scroll(cga_correct_coords(&cga_x, &cga_y));
    cga_set_cursor(cga_x, cga_y);
}

void cga_puts_color(const char *str, cga_color_t color)
{
    int offset;

    int x = cga_x;
    int y = cga_y;

    while(*str != '\0')
    {
        switch(*str)
        {
            case '\b':
                x--;
                break;

            case '\n':
                y++;
                x = 0;
                break;

            default:
                offset = cga_off(x++, y);
                cga_mem[offset].id = *str;
                cga_mem[offset].color = color;
                break;
        }

        cga_scroll(cga_correct_coords(&x, &y));

        str++;
    }

    cga_set_cursor(x, y);
}

void cga_putc(char c)
{
    cga_putc_color(c, cga_default_color);
}

void cga_puts(const char *str)
{
    cga_puts_color(str, cga_default_color);
}

#endif

