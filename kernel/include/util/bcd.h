#ifndef _BCD_H_
#define _BCD_H_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
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
 *  @author Tom Slawik <tom.slawik@gmail.com>
 */

#include <printf.h>

// WARNING: ONLY values up to 99
#define BCD_DECODE(x) ((0b1111 & x) + (10 * (((0b1111 << 4) & x) >> 4)))

static inline uint8_t BCD_ENCODE(char *bcd_str, int x)
{
    sprintf(bcd_str, "%d", x);
    return (uint8_t)bcd_str;
}

#endif
