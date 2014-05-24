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
 * @author Michael Sippel (Universe Team) <micha@infinitycoding.com>
 */

#include <stdint.h>
#include <mm/heap.h>
#include <pmm.h>
#include <memory_layout.h>
#include <mm/paging.h>
#include <string.h>

static alloc_t *first_node = NULL;

void INIT_HEAP(void)
{
	// nothing to do here :)
}

/**
 * add an inode to the list
 *
 * @param inode node to add
 */
void heap_add(alloc_t *inode)
{
	inode->next = first_node;
	first_node = inode;
}

/**
 * create an allocation inode with a specific size
 *
 * @param pages number of pages to allocate
 * @return allocation inode
 */
alloc_t *heap_expand(int pages)
{
#ifdef HEAP_DEBUG
	printf("heap_expand(): add %d pages...\n", pages);
#endif

	// allocate memory
    paddr_t pframe = 0;
    vaddr_t vframe = arch_vaddr_find(&current_context->arch_context, pages,
                                     MEMORY_LAYOUT_KERNEL_HEAP_START,
                                     MEMORY_LAYOUT_KERNEL_HEAP_END, VMM_WRITABLE);
    vaddr_t vframe_cur = vframe;

    int i;
    for(i = 0; i < pages; i++)
    {
        pframe = pmm_alloc_page();
        vmm_map(current_context, pframe, vframe_cur, VMM_WRITABLE);
        vframe_cur += PAGE_SIZE;
    }

	// create inode
    alloc_t *new_header = (alloc_t *) vframe;

    new_header->size = pages*PAGE_SIZE - sizeof(alloc_t);
    new_header->base = vframe + sizeof(alloc_t);
    new_header->status = HEAP_STATUS_FREE;

    heap_add(new_header);

    return new_header;
}

/**
 * Search for a free allocation inode and mark it as used.
 * If nothing found, add a new inode.
 *
 * @param size number of bytes
 * @return pointer to reserved bytes
 */
void *malloc(size_t bytes)
{
    alloc_t *header = first_node;
    vaddr_t data = 0;
    int n_size = bytes + sizeof(alloc_t);

	// go through all inodes...
    while(header != NULL)
    {
		// fits the size?
        if(header->size >= bytes && header->status == HEAP_STATUS_FREE)
        {
			// mark as used
            header->status = HEAP_STATUS_USED;

			// if something is left, split it down
            if(header->size > n_size)
            {
                alloc_t *new_header = (alloc_t *)(header->base + bytes);
                new_header->base    = header->base + n_size;
                new_header->size = header->size - n_size;
                new_header->status = HEAP_STATUS_FREE;
                header->size = bytes;

				heap_add(new_header);
            }

            return (void*) header->base;
        }

        header = header->next;
    }

	// if nothing found, create new stuff...
    header = heap_expand(NUM_PAGES(n_size));
	if(header != NULL)
    {
        header->status = HEAP_STATUS_USED;
        return (void *)header->base;
    }

#ifdef HEAP_DEBUG
    printf("malloc(): reserving %d bytes of memory: %p - %p\n", header->size, data, data + header->size);
#endif

	// no more memory :'(
    return NULL;
}

/**
 * Free a range of bytes in th heap
 *
 * @param ptr pointer
 */
void free(void *ptr)
{
	// calculate inode adress
    alloc_t *header = (alloc_t*)((uintptr_t)ptr - sizeof(alloc_t));
	// mark as free
    header->status = HEAP_STATUS_FREE;

#ifdef HEAP_DEBUG
    printf("free(): freeing %d bytes of memory: %p - %p\n", header->size, ptr, ptr + header->size);
#endif
}

/**
 * allocate num*blocks and clear memory
 *
 * @param num number of blocks
 * @param size size of one block
 *
 * @return pointer to allocated memory
 */
void *calloc(size_t num, size_t size)
{
	size_t bytes = num * size;

    void *data = malloc(bytes);
    memset(data, 0, bytes);

    return data;
}

/**
 * resize an allocated object
 *
 * @param ptr pointer to old location
 * @param size new size
 *
 * @return new pointer
 */
void *realloc(void *ptr, size_t size)
{
    void *dest = malloc(size);
    alloc_t *source_alloc = (alloc_t*)((uintptr_t)ptr - sizeof(alloc_t));

#ifdef HEAP_DEBUG
	printf("realloc(): copying %d bytes from 0x%x to 0x%x\n", source_alloc->size, ptr, dest);
#endif

	if(source_alloc->size < size)
	{
		memcpy(dest, ptr, source_alloc->size);
    }

	free(ptr);

    return dest;
}

