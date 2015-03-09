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
 * @file mm/shm.c
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <list.h>
#include <stddef.h>
#include <sys/types.h>
#include <sched/process.h>
#include <sched/thread.h>
#include <memory_layout.h>
#include <mm/heap.h>
#include <mm/paging.h>
#include <mm/shm.h>

static list_t *shm_segments;

extern struct thread_state *current_thread;

void INIT_SHM(void)
{
    shm_segments = list_create();
}

shm_segment_t *shm_create(unsigned int key, size_t size, uid_t uid, gid_t gid)
{
    shm_segment_t *segment = malloc(sizeof(shm_segment_t));

    segment->key = key;
    segment->size = size;
    segment->uid = uid;
    segment->gid = gid;

    segment->master_context = NULL;

    list_push_back(shm_segments, segment);

    return segment;
}

shm_descriptor_t *shm_create_descriptor(struct process_state *process, shm_segment_t *segment)
{
    shm_descriptor_t *descriptor = malloc(sizeof(shm_descriptor_t));

    descriptor->id = list_length(process->shm_descriptors);
    descriptor->segment = segment;

    list_push_back(process->shm_descriptors, descriptor);

    return descriptor;
}

shm_descriptor_t *shm_get_descriptor(struct process_state *process, unsigned int id)
{
    return (shm_descriptor_t*) list_get_by_int(process->shm_descriptors, offsetof(shm_descriptor_t, id), id);
}

shm_segment_t *shm_get(unsigned int key)
{
    iterator_t it = iterator_create(shm_segments);
    list_set_first(&it);

    while(! list_is_last(&it) )
    {
        shm_segment_t *segment = list_get_current(&it);

        if(segment->key == key)
            return segment;
        else
            list_next(&it);
    }

    return NULL;
}

void *shm_attach(vmm_context_t *context, shm_segment_t *segment, uintptr_t offset)
{
    if(offset >= segment->size)
        return -1;

    uintptr_t base = 0xB0000000; /* TODO */
    uintptr_t start = (base + offset) & PAGE_MASK;
    size_t size = segment->size - offset;
    size_t num_pages = NUM_PAGES(size);

    int i;

    if(segment->master_context == NULL)
    {
        segment->master_context = context;
        segment->master_base = base;

        for(i = 0; i < num_pages; i++)
        {
            vaddr_t vaddr = start + i * PAGE_SIZE;
            paddr_t paddr = pmm_alloc_page();
            vmm_map(context, paddr, vaddr, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
        }
    }
    else
    {
        for(i = 0; i < num_pages; i++)
        {
            vaddr_t vaddr = start + i * PAGE_SIZE;
            vaddr_t master_vaddr = segment->master_base + i * PAGE_SIZE;
            paddr_t paddr = arch_vaddr2paddr(segment->master_context, master_vaddr);

            vmm_map(context, paddr, vaddr, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
        }
    }

    return start;
}

void shm_detach(vmm_context_t *context, void *base, size_t size)
{
    uintptr_t start = (uintptr_t)base & PAGE_MASK;
    size_t num_pages = NUM_PAGES(size);
    vmm_unmap_range(context, start, num_pages);
}

void sys_shm_get(struct cpu_state **cpu)
{
    unsigned int key = (*cpu)->CPU_ARG1;
    size_t size = (*cpu)->CPU_ARG2;
    int flags = (*cpu)->CPU_ARG3;

    // TODO: check access rights
    shm_segment_t *segment = shm_get(key);
    if(segment == NULL && flags & SHM_CREAT)
    {
        segment = shm_create(key, size, current_thread->process->uid, current_thread->process->gid);
    }

    if(segment != NULL)
    {
        shm_descriptor_t *desc = shm_create_descriptor(current_thread->process, segment);
        (*cpu)->CPU_ARG0 = desc->id;
    }
    else
        (*cpu)->CPU_ARG0 = -1;
}

void sys_shm_ctl(struct cpu_state **cpu)
{
}

void sys_shm_attach(struct cpu_state **cpu)
{
    unsigned int id = (*cpu)->CPU_ARG1;
    const void *base = (*cpu)->CPU_ARG2;
    int flags = (*cpu)->CPU_ARG3;

    shm_descriptor_t *desc = shm_get_descriptor(current_thread->process, id);
    if(desc != NULL)
    {
        desc->size = desc->segment->size - (size_t) base;
        desc->base = shm_attach(&current_thread->context.memory, desc->segment, base);
        (*cpu)->CPU_ARG0 = desc->base;
    }
    else
        (*cpu)->CPU_ARG0 = -1;
}

void sys_shm_detach(struct cpu_state **cpu)
{
    void *base = (*cpu)->CPU_ARG0;

    shm_descriptor_t *desc = list_get_by_int(current_thread->process->shm_descriptors, offsetof(shm_descriptor_t, base), base);

    if(desc != NULL)
    {
        shm_detach(&current_thread->context.memory, desc->base, desc->size);
        (*cpu)->CPU_ARG0 = 0;
    }
    else
        (*cpu)->CPU_ARG0 = -1;
}

