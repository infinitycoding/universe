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
#include <mm/vmm.h>
#include <mm/layout.h>

#include <gdt.h>

// global stuff
struct process_state *kernel_state = NULL;	// kernel init thread state
struct thread_state *current_thread = NULL; // current thread state
list_t *running_threads = NULL;				// threads to be scheduled
list_t *process_list = NULL;				// list of all processes created
list_t *zombie_list = NULL;					// list of zomibe pids
pid_t pid_counter = 1;						// counter to generate next pid

iterator_t thread_iterator;

/**
 * @brief	Initiate the scheduler module
 */
void INIT_SCHEDULER(void)
{
    running_threads = list_create();
    process_list = list_create();
    zombie_list = list_create();
    thread_iterator = iterator_create(running_threads);


    // create kernel process
    kernel_state = process_create("Kernel INIT", PROCESS_ACTIVE, NULL, 0, 0);
    current_thread = kernel_thread_create(NULL, NULL, NULL);

    // map kernel stack
    vmm_map(current_context, pmm_alloc_page(), 0xFFFFF000, VMM_PRESENT | VMM_WRITABLE);
    vmm_map(current_context, pmm_alloc_page(), 0xFFFFE000, VMM_PRESENT | VMM_WRITABLE);
    vmm_map(current_context, pmm_alloc_page(), 0xFFFFD000, VMM_PRESENT | VMM_WRITABLE);
    vmm_map(current_context, pmm_alloc_page(), 0xFFFFC000, VMM_PRESENT | VMM_WRITABLE);

    // enable multitasking
    enable_irqs();
}

/**
 * @brief			performs context switches
 * @param thread	pointer to the thread state to switch to
 * @return			new cpu state
 */
struct cpu_state *task_switch(struct thread_state *thread)
{
    struct cpu_state *cpu = thread->context.state;
    set_kernelstack(cpu+1);
    vmm_switch_context(&thread->context.memory);
    set_iobmp(&thread->context);
    return cpu;
}

/**
 * @brief		selects the next thread and calls a context switch
 * @param cpu	current cpu state
 * @return		new cpu state
 */
struct cpu_state *task_schedule(struct cpu_state *cpu)
{
    if(current_thread == NULL)
    {
        current_thread = list_get_current(&thread_iterator);
        current_thread->ticks = 0;
    }
    else
    {
        current_thread->context.state = cpu;
    }

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

        cpu = task_switch(current_thread);
    }
    else if(current_thread->ticks == 0)
    {
        current_thread->ticks = 10;
        list_next(&thread_iterator);
        if(list_is_last(&thread_iterator))
        {
            list_set_first(&thread_iterator);
        }
        current_thread = list_get_current(&thread_iterator);

        cpu = task_switch(current_thread);
    }
    else
    {
        current_thread->ticks--;
    }

    return cpu;
}

