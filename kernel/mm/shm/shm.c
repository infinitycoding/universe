/*
     Copyright 2015 Infinitycoding all rights reserved
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
 * @file mm/shm/shm.c
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <list.h>
#include <stddef.h>
#include <types.h>
#include <sched/process.h>
#include <sched/thread.h>
#include <memory_layout.h>
#include <mm/heap.h>
#include <mm/paging.h>
#include <mm/shm.h>

/**
 *	size in pages
 */
shm_context_t *shm_create_context(size_t size)
{
    shm_context_t *context = malloc(sizeof(shm_context_t));
    context->size = size;
    context->phys_pages = list_create();

    int i;
    for(i = 0; i < size; i++)
    {
        list_push_back(context->phys_pages, pmm_alloc_page());
    }

    return context;
}

void shm_destroy_context(shm_context_t *context)
{
    iterator_t it = iterator_create(context->phys_pages);
    list_set_first(&it);
    while( ! list_is_last(&it) )
    {
        void *addr = list_get_current(&it);
        pmm_mark_page_as_free(addr);

        list_next(&it);
    }

    free(context);
}

void *shm_attach(vmm_context_t *vmm_context, shm_context_t *shm_context, uintptr_t offset)
{
    if(offset >= shm_context->size)
        return -1;

    uintptr_t base = 0xB0000000; /* TODO */
    uintptr_t start = (base + offset) & PAGE_MASK;

    iterator_t it = iterator_create(shm_context->phys_pages);
    list_set_first(&it);

    int i = 0;
    while( ! list_is_last(&it) )
    {
        vaddr_t vaddr = start + i * PAGE_SIZE;
        paddr_t paddr = list_get_current(&it);

        vmm_map(vmm_context, paddr, vaddr, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
        i++;
    }

    return start;
}

void shm_detach(vmm_context_t *context, shm_descriptor_t *desc)
{
    uintptr_t start = (uintptr_t)desc->base & PAGE_MASK;
    size_t num_pages = desc->context->size;
    vmm_unmap_range(context, start, num_pages);
}

