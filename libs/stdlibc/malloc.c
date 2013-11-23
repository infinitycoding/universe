/*
	Copyright 2013 universe coding group (UCG) all rights reserved
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
	@author Michael Sippel (Universe Team) <michamimosa@gmail.com>
*/
#include <stdlib.h>
#include <stdint.h>

#define HEAP_STATUS_FREE 0x0
#define HEAP_STATUS_USED 0x1

typedef struct alloc {
	size_t size;
	uint32_t base;
	int status;	

	struct alloc *next;
} alloc_t;

static alloc_t *first_node = NULL;

alloc_t *heap_expand(int pages) {
	uint32_t vframe = alloc_memory(pages);
	alloc_t *new_header = (alloc_t *) vframe;

	new_header->size = pages*4096 - sizeof(alloc_t);
	new_header->base = vframe + sizeof(alloc_t);
	new_header->status = HEAP_STATUS_FREE;

	new_header->next = first_node;
	first_node = new_header;
	
	return new_header;
}

void *malloc(size_t size) {
	alloc_t *header = first_node;
	vaddr_t data = 0;
	int n_size = size + sizeof(alloc_t);

	while(header != NULL) {
		if(header->size >= size && header->status == HEAP_STATUS_FREE) {
			header->status = HEAP_STATUS_USED;
			/*if(header->size > n_size) {
				alloc_t *new_header = header->base + size;
				new_header->base    = header->base + n_size;
				new_header->size = header->size - n_size;
				new_header->status = HEAP_STATUS_FREE;
				header->size = size;

				new_header->next = heap->alloc_list;
				heap->alloc_list = new_header;
			}*/

			return (void *)header->base;
		}
		header = header->next;
	}

	header = heap_expand(NUM_PAGES(n_size));
	if(header != NULL) {
		header->status = HEAP_STATUS_USED;
		return (void *)header->base;
	}

	return NULL;
}

void free(void *ptr) {
	alloc_t *header = (alloc_t*)((uintptr_t)ptr - sizeof(alloc_t));
	header->status = HEAP_STATUS_FREE;
}


void *calloc(size_t num, size_t size) {
	void *data = malloc(size);
	memset(data, 0, size);

	return data;
}

void *realloc(void *ptr, size_t size) {
	void *dest = malloc(size);
	alloc_t *source_alloc = ptr - sizeof(alloc_t);

	memmove(dest, ptr, source_alloc->size);
	free(ptr);

	return dest;
}


