#ifndef _HEAP_H_
#define _HEAP_H_

/*
	Copyright 2012-2014 universe coding group (UCG) all rights reserved
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
 *  @author Michael Sippel (Universe Team) <micha@infinitycoding.com>
 */

#include <stdint.h>

/*
	std interface
*/

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);

/*
	internal interface
*/

#define HEAP_STATUS_USED 0
#define HEAP_STATUS_FREE 1

typedef struct alloc
{
    size_t size;
    vaddr_t base;
    int status;

    struct alloc *next;
} alloc_t;

void INIT_HEAP(void);
alloc_t *heap_expand(int pages);
void *heap_alloc(size_t size);
void heap_free(void *ptr);

#endif

