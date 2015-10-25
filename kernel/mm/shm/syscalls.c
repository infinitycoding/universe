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
 * @file mm/shm/syscalls.c
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <list.h>
#include <stddef.h>
#include <types.h>
#include <sched/process.h>
#include <sched/thread.h>
#include <mm/layout.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/shm.h>

/**
 * doesn't work yet
 */

extern struct thread_state *current_thread;


void sys_kill(struct cpu_state **cpu)
{
    unsigned int id = (*cpu)->CPU_ARG1;
    unsigned int sig = (*cpu)->CPU_ARG2;

    shm_descriptor_t *desc = shm_get_descriptor(current_thread->process, id);

    iterator_t it = iterator_create(desc->context->users);
    list_set_first(&it);
    while(! list_is_last(&it))
    {
        struct process_state *proc = list_get_current(&it);
        if(proc != NULL && proc != current_thread->process)
            send_signal(proc, sig);

        list_next(&it);
    }

    (*cpu)->CPU_ARG0 = 0;
}


void sys_shm_get(struct cpu_state **cpu)
{
    unsigned int key = (*cpu)->CPU_ARG1;
    size_t size = (*cpu)->CPU_ARG2;
    int flags = (*cpu)->CPU_ARG3;

    // TODO: check access rights
    shm_context_t *context = NULL; // TODO !!!!!!!
    if(context == NULL)
    {
        context = shm_create_context(size);
    }

    if(context != NULL)
    {
        shm_descriptor_t *desc = shm_create_descriptor(current_thread->process, context);
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
        desc->base = shm_attach(&current_thread->context.memory, desc->context, base);
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
        shm_detach(&current_thread->context.memory,	desc);
        (*cpu)->CPU_ARG0 = 0;
    }
    else
        (*cpu)->CPU_ARG0 = -1;
}

