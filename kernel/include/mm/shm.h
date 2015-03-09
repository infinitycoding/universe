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

#define SHM_CREAT 0x1

typedef struct shm_segment
{
    unsigned int key;
    size_t size;

    gid_t uid;
    uid_t gid;

    vmm_context_t *master_context;
    void *master_base;
} shm_segment_t;

typedef struct shm_descriptor
{
    unsigned int id;
    shm_segment_t *segment;

    void *base;
    size_t size;
} shm_descriptor_t;

void INIT_SHM(void);

shm_segment_t *shm_create(unsigned int key, size_t size, uid_t uid, gid_t gid);
shm_segment_t *shm_get(unsigned int key);

shm_descriptor_t *shm_create_descriptor(struct process_state *process, shm_segment_t *segment);
shm_descriptor_t *shm_get_descriptor(struct process_state *process, unsigned int id);

void *shm_attach(vmm_context_t *context, shm_segment_t *segment, uintptr_t offset);
void shm_detach(vmm_context_t *context, void *base, size_t size);

void sys_shm_get(struct cpu_state **cpu);
void sys_shm_ctl(struct cpu_state **cpu);
void sys_shm_attach(struct cpu_state **cpu);
void sys_shm_detach(struct cpu_state **cpu);

#endif

