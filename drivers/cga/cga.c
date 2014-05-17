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
 *  @author Tom Slawik <tom.slawik@gmail.com>
 */

#include <udrcp.h>
#include <cga.h>
#include <string.h>


void gotoxy(struct crtc_handle *crtc, struct cga_handle *cga, uint8_t _x, uint8_t _y, int color)
{
     uint16_t offset;

     if ((_y * cga->cols + _x) >= (cga->cols * cga->lines))
     { /* scroll if neccessary */
          memmove(cga->video_mem, cga->video_mem + 2 * cga->cols, 2 * cga->lines * cga->cols);
          memset(cga->video_mem + 2 * cga->lines * cga->cols, 0, 2 * cga->cols);
          --_y;
     }

     cga->xpos = _x; 
     cga->ypos = _y;
     offset = _y * cga->cols + _x;

     if (cga->video_mem[2 * offset] == 0)
     {
          cga->video_mem[2 * offset + 1] = color;
     }

     crtc_write(crtc, CRTC_CURSOR_LOCATION_HIGH, (uint8_t)(offset >> 8));
     crtc_write(crtc, CRTC_CURSOR_LOCATION_LOW, (uint8_t)offset);
}

int putchar(struct crtc_handle *crtc, struct cga_handle *cga, int c, int color)
{
     if (c == '\b') {
          if (cga->xpos > 0) {
               gotoxy(crtc, cga, --cga->xpos, cga->ypos, color);
          } else {
               gotoxy(crtc, cga, cga->cols - 1, --cga->ypos, color);
          }

          cga->video_mem[2 * (cga->ypos * cga->cols + cga->xpos)] = 0;

          return c;
     }

     if ((cga->xpos > cga->cols - 1) || (c == '\n')) {
          gotoxy(crtc, cga, 0, ++cga->ypos, color);

          if (c == '\n') {
               return c;
          }
     }

     cga->video_mem[2 * (cga->ypos * cga->cols + cga->xpos)] = c;
     cga->video_mem[2 * (cga->ypos * cga->cols + cga->xpos) + 1] = color;

     gotoxy(crtc, cga, ++cga->xpos, cga->ypos, color);

     return c;
}

