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
	@author Michael Sippel (Universe Team) <micha.linuxfreak@gmail.com>
*/

#include <stdint.h>
#include <heap.h>
#include <pmm.h>
#include <memory_layout.h>
#include <paging.h>
#include <string.h>

heap_t kernel_heap;

void INIT_HEAP(void) {
	heap_init(&kernel_heap);
}

/* c standard interface. do some preparation here! */

void *malloc(size_t size) {
	return heap_alloc(&kernel_heap, size);
}

void free(void * ptr) {
	heap_free(&kernel_heap, ptr);
}

void *calloc(size_t num, size_t size) {
	void *data = heap_alloc(&kernel_heap, size);
	memset(data, 0, size);

	return data;
}

void *realloc(void *ptr, size_t size) {
	void *dest = heap_alloc(&kernel_heap, size);
	alloc_t *source_alloc = ptr - sizeof(alloc_t);

	memmove(dest, ptr, source_alloc->size);
	free(ptr);

	return dest;
}

/* implementation */

/**
 * Initalize a heap
 *
 * @param heap heap to initalise
 * @return void
 */
void heap_init(heap_t *heap) {
	vaddr_t vframe = vmm_automap_kernel(current_context, pmm_alloc_page(), VMM_WRITABLE);
	alloc_t *header = (alloc_t *)vframe;

	header->size = PAGE_SIZE - sizeof(alloc_t);
	header->base = vframe + sizeof(alloc_t);
	header->status = HEAP_STATUS_FREE;
	header->next = NULL;

	heap->list_count = 1;
	heap->alloc_list = header;
}

/**
 * Add a page to the heap
 *
 * @param heap heap
 * @return void
 */
alloc_t *heap_expand(heap_t *heap, int pages) {
	/* TODO : Range Allocation */
	heap->list_count++;
	paddr_t pframe = 0;
	vaddr_t vframe = arch_vaddr_find(&current_context->arch_context, pages,
				    MEMORY_LAYOUT_KERNEL_HEAP_START,
				    MEMORY_LAYOUT_KERNEL_HEAP_END, VMM_WRITABLE);
	vaddr_t vframe_cur = vframe;

	int i;
	for(i = 0; i < pages; i++) {
		pframe = pmm_alloc_page();
		vmm_map(current_context, pframe, vframe_cur, VMM_WRITABLE);
		vframe_cur += PAGE_SIZE;
	}

	alloc_t *new_header = (alloc_t *) vframe;

	new_header->size = pages*PAGE_SIZE - sizeof(alloc_t);
	new_header->base = vframe + sizeof(alloc_t);
	new_header->status = HEAP_STATUS_FREE;

	new_header->next = heap->alloc_list;
	heap->alloc_list = new_header;
	heap->list_count ++;
	return new_header;
}

/**
 * Destroy a heap
 *
 * @param heap heap
 * @return void
 */
void heap_destroy(heap_t *heap) {
	while (heap->list_count != 0) {
		vmm_unmap(current_context,
			 heap->alloc_list[--heap->list_count].base & ~0xFFF);
	}
}

/**
 * Find a free range of bytes in the heap and reserve this
 *
 * @param heap heap
 * @param size number of bytes
 *
 * @return pointer to reserved bytes
 */
void *heap_alloc(heap_t *heap, size_t size) {
	alloc_t *header = heap->alloc_list;
	vaddr_t data = 0;
	int n_size = size + sizeof(alloc_t);

	if(size <= PAGE_SIZE && size > PAGE_SIZE - sizeof(alloc_t)) {
		data = vmm_automap_kernel(current_context, pmm_alloc_page(), VMM_WRITABLE);
		return (void *)data;
	}

	while(header != NULL) {
		if(header->size >= size && header->status == HEAP_STATUS_FREE) {
			header->status = HEAP_STATUS_USED;
			if(header->size > n_size) {
				alloc_t *new_header = (alloc_t *)(header->base + size);
				new_header->base    = header->base + n_size;
				new_header->size = header->size - n_size;
				new_header->status = HEAP_STATUS_FREE;
				header->size = size;

				new_header->next = heap->alloc_list;
				heap->alloc_list = new_header;
			}

			return (void *)header->base;
		}
		header = header->next;
	}

	header = heap_expand(heap, NUM_PAGES(n_size));
	if(header != NULL) {
		header->status = HEAP_STATUS_USED;
		return (void *)header->base;
	}

	#ifdef HEAP_DEBUG
		printf("heap_alloc(): reserving %d bytes of memory: %p - %p\n", header->size, data, data + header->size);
	#endif

	return NULL;
}

/**
 * Free a range of bytes in th heap
 *
 * @param heap heap
 * @param ptr pointer
 *
 * @return void
 */
void heap_free(heap_t *heap, void *ptr) {
	alloc_t *header = (alloc_t*)((uintptr_t)ptr - sizeof(alloc_t));
	header->status = HEAP_STATUS_FREE;

	#ifdef HEAP_DEBUG
		printf("heap_free(): freeing %d bytes of memory: %p - %p\n", header->size, ptr, ptr + header->size);
	#endif
}