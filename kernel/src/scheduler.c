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
#include <task.h>
#include <thread.h>
#include <drivers/timer.h>
#include <tss.h>
#include <gdt.h>
#include <pmm.h>
#include <heap.h>
#include <string.h>

tss_s tss = { .ss0 = 0x10 };

uint32_t *kernelstack;

struct task_state *proc0;
struct task_state *currentprocess;

bool sched_lock = false;

extern pd_t *pd_kernel;
extern pd_t *pd_current;

void test(void)
{

    while(1){printf("wtf");}
}

void INIT_SCHEDULER(void)
{
	set_GDT_entry(5, (uint32_t) &tss, sizeof(tss), 0x89, 0x8);
	load_gdt(5);
	asm ("ltr %%ax" : : "a" (5 << 3));
	kernelstack = malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
	tss.esp = (uint32_t)kernelstack;

    proc0 = malloc(sizeof(proc0[0]));
    proc0->pagedir = pd_kernel;
    proc0->flags  = TASK_ACTIV|TASK_REALTIME_PRIORITY;
    strncpy(proc0->name,"kernel",256);
    strncpy(proc0->desc,"Kernel",256);

    proc0->next = proc0->next;
    proc0->prev = proc0;
    proc0->next = proc0;
    proc0->pid = get_freepid();

    currentprocess = proc0;
	thread_create(currentprocess, 0, 0);
	
}

struct cpu_state *task_schedule(struct cpu_state *cpu)
{
    currentprocess->currentthread->thread_state = cpu;
    currentprocess->currentthread = currentprocess->currentthread->next;
    printf("%d",currentprocess->currentthread->tid);
    //while(!currentprocess->currentthread->flags&THREAD_ACTIV) currentprocess->currentthread = currentprocess->currentthread->next;
    cpu = currentprocess->currentthread->thread_state;

	EOI(0);
	return cpu;
}
