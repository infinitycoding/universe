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
 *  @file /mm/heap.c
 *  @brief Kernel Module for Dynamic memory management (malloc, free)
 *  @author Michael Sippel (Universe Team) <micha@infinitycoding.com>
 */

#include <stdint.h>
#include <mm/heap.h>
#include <pmm.h>
#include <memory_layout.h>
#include <mm/paging.h>
#include <string.h>
#include <printf.h>

static struct header_block *used_blocks = NULL;
static struct header_block *free_blocks = NULL;



/**
 *  @brief Initiates the dynamic memory management module.
 */
void INIT_HEAP(void)
{
    used_blocks = create_block();
    free_blocks = create_block();
    free_blocks->fragments[0].base = MEMORY_LAYOUT_KERNEL_HEAP_START;
    free_blocks->fragments[0].size = MEMORY_LAYOUT_KERNEL_HEAP_END - MEMORY_LAYOUT_KERNEL_HEAP_START;
}


/**
 *  @brief Allocates header block for internal memory management structs.
 *  @return Pointer to the new header block
 */
struct header_block *create_block(void)
{
    struct header_block *block = (struct header_block *) vmm_automap_kernel(current_context, pmm_alloc_page(), VMM_WRITABLE);
    memset((void*)block, 0, PAGE_SIZE);

    return block;
}


/**
 *  @brief Adds a Memory fragment to management struct.
 *  @param header The Header block.
 *  @param base   The base adress of the new fragment.
 *  @param size   The size of the new Fragment.
 */
void heap_add_fragment(struct header_block *header, vaddr_t base, size_t size)
{
    // go through all header blocks...
    while(header != NULL)
    {
        int i;

        // go through all fragments...
        for(i = 0; i < 511; i++)
        {
            if(header->fragments[i].base == 0)
            {
                header->fragments[i].base = base;
                header->fragments[i].size = size;
#if HEAP_DEBUG
                printf("[heap/add] %p, %d to %p\n", base, size, header);
#endif
                return;
            }
        }

        if(header->next == NULL)
        {
            header->next = create_block();
        }

        header = header->next;
    }
}


/**
 *  @brief Removes a fragment from the management struct
 *  @param header Header Block.
 *  @param base   Base address of the fragment to remove.
 *  @return size  0 = failure else the size fo the removed fragment
 */
size_t heap_remove_fragment(struct header_block *header, vaddr_t base)
{
    // go through all header blocks...
    while(header != NULL)
    {
        int i;

        // go through all fragments...
        for(i = 0; i < 511; i++)
        {
            if(header->fragments[i].base == base)
            {
                header->fragments[i].base = 0;
#if HEAP_DEBUG
                printf("[heap/remove] %p, %d from %p\n", base, header->fragments[i].size, header);
#endif
                return header->fragments[i].size;
            }
        }

        header = header->next;
    }

    return 0;
}


/**
 *  @brief provide a specific data area
 *  @param start pointer to begin of data
 *  @param end pointer to end of data
 */
void heap_provide_address(vaddr_t start, vaddr_t end)
{
    int pages = NUM_PAGES(end - (start & PAGE_MASK));

    paddr_t pframe = 0;
    vaddr_t vframe = start & PAGE_MASK;

    int i;
    for(i = 0; i < pages; i++)
    {
        if(! arch_vmm_is_present(&current_context->arch_context, vframe) &&
                arch_vaddr2paddr(&current_context->arch_context, vframe) == 0)
        {
            pframe = pmm_alloc_page();
            vmm_map(current_context, pframe, vframe, VMM_WRITABLE);
        }
        vframe += PAGE_SIZE;
    }
}

#
/**
 * @brief Search for a free address and mark it as used.
 * @param size number of bytes
 * @return pointer to reserved bytes
 */
void *malloc(size_t bytes)
{
    struct header_block *header = free_blocks;

    // go through all header blocks...
    while(header != NULL)
    {
        int i;

        // go through all fragments...
        for(i = 0; i < 511; i++)
        {
            if(header->fragments[i].base == 0)
            {
                continue;
            }

            if(header->fragments[i].size >= bytes)
            {
                // found some space.
                vaddr_t base = header->fragments[i].base;

#if HEAP_DEBUG
                printf("[heap/malloc] %p (%d >= %d) in %p\n", base, header->fragments[i].size,bytes, header);
#endif

                if(header->fragments[i].size > bytes)
                {
                    // shrink fragment
                    header->fragments[i].base += bytes;
                    header->fragments[i].size -= bytes;
                }
                else
                {
                    // remove fragment
                    header->fragments[i].base = 0;
                }

                // add fragment to used list
                heap_add_fragment(used_blocks, base, bytes);

                // make sure that everything is mapped
                heap_provide_address(base, base + bytes);

                return (void *)base;
            }
        }

        header = header->next;
    }

    // no more memory :'(
    printf("!!! ERROR: no memory for malloc.. !!!\n");
    while(1);

    return NULL;
}


/**
 *  @brief Free a range of bytes in the heap
 *  @param ptr pointer
 */
void free(void *ptr)
{
    size_t bytes = heap_remove_fragment(used_blocks, (vaddr_t) ptr);
    heap_add_fragment(free_blocks, (vaddr_t)ptr, bytes);
}


/**
 * @brief allocate num*blocks and clear memory
 * @param num number of blocks
 * @param size size of one block
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
 *  @brief resize an allocated object
 *  @param ptr pointer to old location
 *  @param size new size
 *  @return new pointer
 */
void *realloc(void *ptr, size_t size)
{
    // get fragment size and remove
    size_t old_size = heap_remove_fragment(used_blocks, (vaddr_t) ptr);

    // malloc new
    void *dest = malloc(size);

    // copy data from old to new
    size_t copy_size = (size > old_size) ? old_size : size;
    memcpy(dest, ptr, copy_size);

    return dest;
}

