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

bool test_heap(int na, int nb)
{
    int i;
    printf("allocating array a with %d integers...\n", na);
    int *a = malloc(sizeof(int) * na);
    printf("a is at 0x%p\nwriting to a...", a);

    for(i = 0; i < na; i++) a[i] = i;

    printf("allocating array b with %d integers...\n", nb);
    int *b = malloc(sizeof(int) * nb);
    printf("b is at 0x%p\nwriting to b...\n", b);
    for(i = 0; i < nb; i++) b[i] = nb-i;

    printf("reallocating array a with %d...\n", nb);
    a = realloc(a, sizeof(int) * nb);

    printf("copying array b to array a...\n");
    for(i = 0; i < nb; i++) a[i] = b[i];

    printf("freeing b\n");
    free(b);

    for(i = 0; i < nb; i++)
	{
		if(nb-i != a[i])
		{
			printf("failure.\n");
			return false;
		}
	}

    printf("freeing a\n");
    free(a);

    return true;
}

