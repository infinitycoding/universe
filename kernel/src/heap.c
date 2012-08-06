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
#include <heap.h>
#include <pmm.h>
#include <paging.h>

heap_t kernel_heap;

void INIT_HEAP(void)
{
	heap_init(&kernel_heap);
}

/* c standard interface. do some preparation here! */

void * malloc ( size_t size )
{
	return heap_alloc(&kernel_heap, size);
}

void free ( void * ptr )
{
	heap_free(&kernel_heap, ptr);
}

void * calloc ( size_t num, size_t size )
{
	void *data = heap_alloc(&kernel_heap, size);
	
	memset(data, 0, size);
	
	return data;
}

void * realloc ( void * ptr, size_t size )
{
	void *dest = heap_alloc(&kernel_heap, size);
	struct alloc_t *source_alloc = ptr - sizeof(struct alloc_t);
	
	memmove(dest, ptr, source_alloc->size);
	free(ptr);
	
	return dest;
}

/* implementation */

void heap_init(heap_t *heap)
{
	heap->page_count = 0;
	heap_expand(heap, 1);
	
	heap->alloc_list = heap->pages[0];
	heap->alloc_list->size = 0;
	heap->alloc_list->prev = heap->alloc_list;
	heap->alloc_list->next = heap->alloc_list;
}

void heap_expand(heap_t *heap, int pages)
{
	/* TODO : Range Allocation + Virtual Memory */
	/* BUG: After 10 Allocations, it overwrites the heap structure */
	heap->pages[heap->page_count++] = (void *)pmm_alloc_page_limit(0xC0000000);
}

void heap_destroy(heap_t *heap)
{
	while (heap->page_count != 0) {
		pmm_mark_page_as_free((paddr_t)heap->pages[--heap->page_count]);
	}
}

void * heap_alloc(heap_t *heap, size_t size)
{
	struct alloc_t *header;
	void *data;
	
	/* step 1: get destination address */
	if (heap->alloc_list->size == 0) { /* first alloc */
		header = heap->alloc_list;
	} else {
		/* TODO: "Best fit" search algorithm for less memory waste */
		header = (struct alloc_t *)((uintptr_t)heap->alloc_list->prev + sizeof(struct alloc_t) + heap->alloc_list->prev->size);
	}
	
	data = (void *)((uintptr_t)header + sizeof(struct alloc_t));
	
	/* step 2: expand heap (if neccessary) */
	unsigned int page = heap->page_count - 1;
	while (data + size > heap->pages[heap->page_count - 1] + PAGE_SIZE) {
		heap_expand(heap, 1);
		header = heap->pages[page];
		data = (void *)((uintptr_t)header + sizeof(struct alloc_t));
	}
	
	/* step 3: set header */
	header->size = size;
	header->prev = heap->alloc_list->prev;
	header->next = heap->alloc_list;
	
	/* step 4: update nodes */
	heap->alloc_list->prev = header;
	
	/* step 5: print debug information */
	#ifdef HEAP_DEBUG
		printf("heap_alloc(): reserving %d bytes of memory: %p - %p\n", header->size, data, data + header->size);
	#endif
	
	return data;
}

void heap_free(heap_t *heap, void *ptr)
{
	struct alloc_t *header = (struct alloc_t *)((uintptr_t)ptr - sizeof(struct alloc_t));
	header->prev->next = header->next;
	header->next->prev = header->prev;
	
	#ifdef HEAP_DEBUG
		printf("heap_free(): freeing %d bytes of memory: %p - %p\n", header->size, ptr, ptr + header->size);
	#endif
}
