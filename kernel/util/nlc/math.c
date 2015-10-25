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
 *  @author Adrian Jablonski <support@quadsoft.org>
 */

#include <math.h>




int powi(int base, int exponent)
{
    int result = base;
    int i;

    if (exponent == 0)
    {
        result = 1;
    }
    else
    {
        for (i = 0; i < exponent - 1; ++i)
        {
            result *= base;
        }
    }

    return result;
}


int abs(int x)
{
    if (x < 0)
    {
        return -x;
    }
    else
    {
        return x;
    }
}

unsigned int bit_scan_forward(unsigned int i)
{
    unsigned int j;

    for (j = 0; j < 32; j++)
    {
        if (i & (1 << j))
        {
            return j;
        }
    }
    return -1;
}
