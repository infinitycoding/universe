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
 * @file mm/shm.c
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <list.h>
#include <sys/types.h>
#include <mm/heap.h>
#include <mm/shm.h>

static list_t *shm_segments;

void INIT_SHM(void)
{
    shm_segments = list_create();
}

shm_segment_t *shm_create(unsigned int key, void *base, size_t size, uid_t uid, gid_t gid)
{
    shm_segment_t *segment = malloc(sizeof(shm_segment_t));

    segment->key = key;
    segment->base = base;
    segment->size = size;
    segment->uid = uid;
    segment->gid = gid;

    list_push_back(shm_segments, segment);

    return segment;
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

void sys_shm_get(struct cpu_state **cpu)
{
}

void sys_shm_ctl(struct cpu_state **cpu)
{
}

void sys_shm_attach(struct cpu_state **cpu)
{
}

void sys_shm_detach(struct cpu_state **cpu)
{
}

