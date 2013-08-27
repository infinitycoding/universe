/*
	Copyright 2012 universe coding group (UCG) all rights reserved
	This file is part of the Universe Kernel.

	Universe Kernel is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	Universe Kernel is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
	@author Simon Diepold aka. Tdotu (Universe Team) <simon.diepold@infinitycoding.de>
	@author Michael Sippel (Universe Team) <micha.linuxfreak@gmail.com>
*/



#include <scheduler.h>


#include <drivers/timer.h>
#include <tss.h>
#include <gdt.h>
#include <pmm.h>
#include <heap.h>
#include <string.h>

tss_s tss = { .ss0 = 0x10 };

uint32_t *kernelstack;

struct process_state *kernel_state;
struct thread_state *current_thread;
list_t *running_threads;

extern pd_t *pd_kernel;
extern pd_t *pd_current;

extern list_t *process_list;
extern list_t *zombie_list;

void INIT_SCHEDULER(void)
{
	set_GDT_entry(5, (uint32_t) &tss, sizeof(tss), 0x89, 0x8);
	load_gdt(5);
	asm volatile("ltr %%ax" : : "a" (5 << 3));
	kernelstack = malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
	tss.esp = (uint32_t)kernelstack;

    running_threads = list_create();
    process_list = list_create();
    zombie_list = list_create();
    kernel_state = process_create("Kernel INIT", "initiate system", PROCESS_ACTIVE, NULL);
    current_thread = thread_create(kernel_state, KERNELMODE, 0, NULL);

}

struct cpu_state *task_schedule(struct cpu_state *cpu)
{
    current_thread->state = cpu;
    if(current_thread->ticks == 0)
    {
        current_thread->ticks = 10;
        list_next(running_threads);
        if(list_is_last(running_threads))
            list_set_first(running_threads);
        current_thread = list_get_current(running_threads);
        cpu = current_thread->state;
        //pd_switch(current_thread->process->pagedir);
    }
    else
    {
        current_thread->ticks--;
    }
	EOI(0);
	return cpu;
}
