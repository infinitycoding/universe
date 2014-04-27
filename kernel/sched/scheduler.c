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
 *	@author Michael Sippel <micha.linuxfreak@gmail.com>
*/

#include <sched/scheduler.h>

#include <string.h>
#include <printf.h>

#include <mm/heap.h>
#include <mm/paging.h>

#include <idt.h>
#include <tss.h>
#include <gdt.h>
#include <pmm.h>



tss_s tss = { .ss0 = 0x10 };

uint32_t *kernelstack;

struct process_state *kernel_state;
struct thread_state *current_thread;
list_t *running_threads;

extern list_t *process_list;
extern list_t *zombie_list;

/**
 * Initiate the scheduler module
 */
void INIT_SCHEDULER(void)
{
	set_GDT_entry(5, (uint32_t) &tss, sizeof(tss), 0x89, 0x8); //qemu does not support TSS-Desc on position 7... wiered hardware stuff
	load_gdt(5);

	asm volatile("ltr %%ax" : : "a" (5 << 3));



	kernelstack = malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
	tss.esp0 = (uint32_t)kernelstack;

    running_threads = list_create();
    process_list = list_create();
    zombie_list = list_create();
    kernel_state = process_create("Kernel INIT", "initiate system", PROCESS_ACTIVE, NULL, 0, 0, NULL);
    current_thread = thread_create(kernel_state, KERNELMODE, 0, NULL, 0, 0,0, NULL);
}


/**
 * performs context switches
 * @param process pointer to the process state
 */
struct cpu_state *task_schedule(struct cpu_state *cpu)
{
    if((current_thread->flags & THREAD_KERNELMODE))
    {
        current_thread->state = cpu;
    }
    else
    {
        memcpy(current_thread->state, cpu, sizeof(struct cpu_state));
    }

    if(current_thread->flags & THREAD_ZOMBIE)
    {
        dump_thread_list(running_threads);
        thread_kill_sub(current_thread);
        if(list_is_empty(running_threads))
        {
            asm volatile("sti");
            while(list_is_empty(running_threads)){printf("halted!\n");}
            asm volatile("cli");
        }

        list_set_first(running_threads);
        current_thread = list_get_current(running_threads);
        vmm_switch_context(&current_thread->context);
        memcpy(cpu, current_thread->state, sizeof(struct cpu_state));
        dump_thread_list(running_threads);
    }
    else if(current_thread->ticks == 0)
    {
        current_thread->ticks = 10;
        list_next(running_threads);
        if(list_is_last(running_threads))
            list_set_first(running_threads);
        current_thread = list_get_current(running_threads);
        vmm_switch_context(&current_thread->context);

        if(current_thread->flags & THREAD_KERNELMODE)
        {
            cpu = current_thread->state;
        }
        else
        {
            cpu = (struct cpu_state *)kernelstack;
            memcpy(cpu, current_thread->state, sizeof(struct cpu_state));
        }
    }
    else
    {
        current_thread->ticks--;
    }
	EOI(0);
	return cpu;
}
