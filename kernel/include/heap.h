#ifndef _heap_h_
#define _heap_h_

/*
	Copyright 2012 universe coding group (UCG) all rights reserved
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
	@author Tom Slawik <tom.slawik@gmail.com>
*/

#include <stdint.h>

/*
	cstdlib interface
*/

void * malloc ( size_t size );
void free ( void * ptr );
void * calloc ( size_t num, size_t size );
void * realloc ( void * ptr, size_t size );

/*
	internal interface
*/

struct alloc_t
{
	int size;
	
	struct alloc_t *prev;
	struct alloc_t *next;
};

typedef struct {
	void *pages[8]; /* WARNING: Actually, it supports only 8 pages :S */
	int page_count;
	
	struct alloc_t *alloc_list;
} heap_t;

void INIT_HEAP(void);
void heap_init(heap_t *heap);
void heap_expand(heap_t *heap, int pages);
void heap_destroy(heap_t *heap);
void * heap_alloc(heap_t *heap, size_t size);
void heap_free(heap_t *heap, void *ptr);
//void * heap_realloc(heap_t *heap, void *ptr, size_t size);

#endif
