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
 *  @author Michael Sippel <micha@infinitycoding.com>
 */

#include <mm/heap.h>
#include <printf.h>

bool test_heap(int n)
{
    int **vals = malloc(n * sizeof(int*));

    int i,j;
    for(i = 0; i < n; i++)
    {
        vals[i] = malloc((i+1) * sizeof(int));
        for(j = 0; j < (i+1); j++)
        {
            vals[i][j] = n + i - j;
        }
    }

    printf("testing....\n");
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < (i+1); j++)
        {
            int expected = n + i - j;
            if(vals[i][j] != expected)
            {
                printf("FAILURE!\n");
            }
        }
    }

    printf("SUCCESS!\n");

    return true;
}

