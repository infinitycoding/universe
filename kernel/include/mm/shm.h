#ifndef _shm_h_
#define _shm_h_

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
#include <types.h>
#include <list.h>
#include <mm/paging.h>
#include <sched/process.h>

typedef struct shm_context
{
    size_t size;
    list_t *phys_pages;
} shm_context_t;

typedef struct shm_descriptor
{
    unsigned int id;
    shm_context_t *context;

    void *base;
} shm_descriptor_t;

void INIT_SHM(void);

shm_context_t *shm_create_context(size_t size);
void shm_destroy_context(shm_context_t *context);
shm_descriptor_t *shm_create_descriptor(struct process_state *process, shm_context_t *context);
shm_descriptor_t *shm_get_descriptor(struct process_state *process, unsigned int id);
void shm_destroy_descriptor(struct process_state *process, unsigned int id);

void *shm_attach(vmm_context_t *vmm_context, shm_context_t *shm_context, uintptr_t offset);
void shm_detach(vmm_context_t *vmm_context, shm_descriptor_t *desc);

void sys_shm_get(struct cpu_state **cpu);
void sys_shm_ctl(struct cpu_state **cpu);
void sys_shm_attach(struct cpu_state **cpu);
void sys_shm_detach(struct cpu_state **cpu);

#endif

