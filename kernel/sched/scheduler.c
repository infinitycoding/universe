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
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *	@author Michael Sippel <micha@infinitycoding.de>
*/

#include <sched/scheduler.h>

#include <string.h>
#include <list.h>
#include <printf.h>

#include <mm/heap.h>
#include <mm/paging.h>

#include <gdt.h>





struct process_state *kernel_state;
struct thread_state *current_thread;
list_t *running_threads;
void *kernelstack;

iterator_t thread_iterator;

extern list_t *process_list;
extern list_t *zombie_list;

/**
 * Initiate the scheduler module
 */
void INIT_SCHEDULER(void)
{
    running_threads = list_create();
    process_list = list_create();
    zombie_list = list_create();
    thread_iterator = iterator_create(running_threads);
    // create kernel process
    kernel_state = process_create("Kernel INIT", PROCESS_ACTIVE, NULL, 0, 0, NULL);
    current_thread = kernel_thread_create(NULL, 0, NULL, NULL);
    kernelstack = malloc(KERNEL_STACK_SIZE) + (KERNEL_STACK_SIZE-sizeof(struct cpu_state));
    set_kernelstack(kernelstack);
    // enable multitasking
    enable_irqs();
}


/**
 * performs context switches
 * @param process pointer to the process state
 */
struct cpu_state *task_schedule(struct cpu_state *cpu)
{
    if((current_thread->flags & THREAD_KERNELMODE))
        current_thread->context.state = cpu;
    else
        memcpy(current_thread->context.state, cpu, sizeof(struct cpu_state));

    if(current_thread->flags & THREAD_ZOMBIE)
    {
        thread_kill_sub(current_thread);
        if(list_is_empty(running_threads))
        {
            enable_irqs();
            while(list_is_empty(running_threads));
            disable_irqs();
        }

        list_set_first(&thread_iterator);
        current_thread = list_get_current(&thread_iterator);
        vmm_switch_context(&current_thread->context.memory);
        memcpy(cpu, current_thread->context.state, sizeof(struct cpu_state));
    }

    else if(current_thread->ticks == 0)
    {
        current_thread->ticks = 10;
        list_next(&thread_iterator);
        if(list_is_last(&thread_iterator))
            list_set_first(&thread_iterator);
        current_thread = list_get_current(&thread_iterator);
        vmm_switch_context(&current_thread->context.memory);

        if(current_thread->flags & THREAD_KERNELMODE)
        {
            cpu = current_thread->context.state;
        }
        else
        {
            cpu = (struct cpu_state *)kernelstack;
            memcpy(cpu, current_thread->context.state, sizeof(struct cpu_state));
        }
    }

    else
    {
        current_thread->ticks--;
    }
    return cpu;
}
