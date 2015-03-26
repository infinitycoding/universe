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
 * @file mm/shm/descriptor.c
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <mm/shm.h>
#include <mm/heap.h>
#include <list.h>
#include <sched/process.h>
#include <stddef.h>

shm_descriptor_t *shm_create_descriptor(struct process_state *process, shm_context_t *context)
{
    shm_descriptor_t *descriptor = malloc(sizeof(shm_descriptor_t));

    descriptor->id = list_length(process->shm_descriptors);
    descriptor->context = context;

    list_push_back(process->shm_descriptors, descriptor);

    return descriptor;
}

shm_descriptor_t *shm_get_descriptor(struct process_state *process, unsigned int id)
{
    return (shm_descriptor_t*) list_get_by_int(process->shm_descriptors, offsetof(shm_descriptor_t, id), id);
}

void shm_destroy_descriptor(struct process_state *process, unsigned int id)
{
    // TODO
}


