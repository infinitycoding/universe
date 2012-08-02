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



    Diese Datei ist ein Teil vom Universe Kernel.

    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <heap.h>
#include <pmm.h>
#include <paging.h>

void heap_test()
{
	heap_t kernel_heap;
	heap_init(&kernel_heap);
	
	uint32_t *test = heap_alloc(&kernel_heap, sizeof(uint32_t));
	printf("%#X", test);
	test = heap_alloc(&kernel_heap, sizeof(uint32_t));
	printf("%#X", test);
	
	//heap_destroy(&kernel_heap);
}

void heap_init(heap_t *heap)
{
	//heap->pages[0] = pd_map_fast(pmm_alloc_page(), PTE_WRITABLE);
	heap->pages[0] = pmm_alloc_page_limit(0xC0000000);
	heap->page_count = 1;
}

void heap_destroy(heap_t *heap)
{
	while (heap->page_count != 0) {
		pmm_mark_page_as_used(heap->pages[--heap->page_count]);
	}
}

void * heap_alloc(heap_t *heap, size_t size)
{
	struct alloc_t alloc;
	alloc.size = size;
	
	memcpy(heap->pages[0], &alloc, sizeof(struct alloc_t));
	
	return heap->pages[0] + sizeof(struct alloc_t);
}
