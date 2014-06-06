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

bool test_heap()
{
    int i;
    printf("allocating array a with 5000 integers...\n");
    int *a = malloc(sizeof(int) * 5000);
    printf("a is at 0x%p\nwriting to a...", a);

    for(i = 0; i < 5000; i++) a[i] = i;

    printf("allocating array b with 25000 integers...\n");
    int *b = malloc(sizeof(int) * 25000);
    printf("b is at 0x%p\nwriting to b...\n", b);
    for(i = 0; i < 25000; i++) b[i] = 25000-i;

    printf("reallocating array a with 25000...\n");
    a = realloc(a, sizeof(int) * 25000);

    printf("copying array b to array a...\n");
    for(i = 0; i < 25000; i++) a[i] = b[i];

    printf("freeing b\n");
    free(b);

    for(i = 0; i < 2500; i++) ;//printf("test : %d, %d\n", i, a[i]); 

    printf("freeing a\n");
    free(a);

    return true;
}