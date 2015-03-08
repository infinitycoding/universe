#ifndef _shm_h
#define _shm_h

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
 * @file /include/mm/shm.h
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <cpu.h>
#include <sys/types.h>

typedef struct shm_segment
{
    unsigned int key;
    void *base;
    size_t size;

    gid_t uid;
    uid_t gid;
} shm_segment_t;

typedef struct shm_descriptor
{
    unsigned int id;
    shm_segment_t *segment;
} shm_descriptor_t;

void INIT_SHM(void);

void sys_shm_get(struct cpu_state **cpu);
void sys_shm_ctl(struct cpu_state **cpu);
void sys_shm_attach(struct cpu_state **cpu);
void sys_shm_detach(struct cpu_state **cpu);

#endif

