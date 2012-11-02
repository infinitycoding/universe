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
#include <printf.h>
#include <string.h>
#include <pmm.h>
#include <paging.h>
#include <heap.h>

tss_s tss = { .ss0 = 0x10 };

uint32_t *kernelstack;

static struct task_state *proc0;
static struct task_state *currentprocess;
static struct zombiepid *freepid = 0;
static uint32_t pit_counter = 0;
static bool lock = false;

extern pd_t *pd_kernel;
extern pd_t *pd_current;

void INIT_SCHEDULER(void)
{
	set_GDT_entry(5, (uint32_t) &tss, sizeof(tss), 0x89, 0x8);
	load_gdt(5);
	asm ("ltr %%ax" : : "a" (5 << 3));
	kernelstack = malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
	tss.esp = (uint32_t)kernelstack;

	//Kernel-init Process
	proc0                 = malloc(TASK_STATE_STRUCT_SIZE);
	proc0->prev           = proc0;
	proc0->next           = proc0;
	proc0->pid            = pit_counter;
	proc0->pagedir        = pd_kernel;
	proc0->port           = NULL;
	proc0->threads        = NULL;
	proc0->proc_children  = NULL;
	proc0->flags          = (ACTIV|REALTIME_PRIORITY | KERNELMODE);
	proc0->tid_counter    = 0;
	proc0->freetid        = NULL;

	strcpy((char*)&proc0->name, "Kernel32.elf");
	strcpy((char*)&proc0->description, "Kernel INIT");
	currentprocess = proc0;
	pd_current = pd_kernel;
	pit_counter++;
	printf("proc0: %#010X next: %#010X\n",proc0, currentprocess);
	//while(1){}
}

struct cpu_state *task_schedule(struct cpu_state *cpu)
{
	if (currentprocess->flags & TRASH) {
		pd_destroy(currentprocess->pagedir);
		free(currentprocess);
	} else {
		//save state
		if (currentprocess->threads) {
			if (!(currentprocess->currentthread->flags & ZOMBIE)) {
				currentprocess->currentthread->thread_state = cpu;
				currentprocess->currentthread = currentprocess->currentthread->next;
			}
		} else {
			currentprocess->main_state = cpu;
		}
	}


	//currentprocess = currentprocess->next;
    /*while (!(currentprocess->flags & ACTIV )) {
		currentprocess=currentprocess->next;
	}*/

	//pd_switch(currentprocess->pagedir);
	//pd_current = currentprocess->pagedir;

	if (currentprocess->threads) {
		while (!(currentprocess->currentthread->flags & (ACTIV|FREEZED))) {
			currentprocess->currentthread = currentprocess->currentthread->next;
		}
		cpu = currentprocess->currentthread->thread_state;
	} else {
		cpu = currentprocess->main_state;
	}


	if (currentprocess->flags & NORMAL_PRIORITY) {
		set_pit_freq(FREQ_NORMAL);
	} else if (currentprocess->flags & REALTIME_PRIORITY) {
		set_pit_freq(FREQ_REALTIME);
	} else {
		set_pit_freq(FREQ_BACKGRUND);
	}
	EOI(0);

	return cpu;
}

pid_t get_freepid(void) {
	pid_t new_pid;

	if (freepid) {
		new_pid = freepid->pid;
		if (freepid->next == freepid) {
			free(freepid);
			freepid = NULL;
		} else {
			freepid->prev->next = freepid->next;
			freepid->next->prev = freepid->prev;
			struct zombiepid *freepid_new = freepid->next;
			free(freepid);
			freepid = freepid_new;
		}
	} else {
		new_pid = pit_counter;
		pit_counter++;
	}

	return new_pid;
}

pid_t proc_create(prev_t prev,vaddr_t vrt_base,paddr_t phy_base,size_t size,vaddr_t entrypoint,char* name,char* desc,priority_t priority) {
	struct task_state *proc_new = malloc(TASK_STATE_STRUCT_SIZE);
	proc_new->pagedir = pd_create();
	memcpy(proc_new->pagedir, pd_kernel, PD_LENGTH<<2);

	uint32_t* stack=malloc(PROC_STACK_SIZE)+PROC_STACK_SIZE;

	if (prev) { //kernelmode
		if (size) {
			pd_map_range(proc_new->pagedir,phy_base,vrt_base,PTE_WRITABLE,size/PAGE_SIZE);
		}
		proc_new->flags = KERNELMODE;
		struct cpu_state new_state = {
			.gs       = 0x10,
			.fs       = 0x10,
			.es       = 0x10,
			.ds       = 0x10,
			.edi      = 0,
			.esi      = 0,
			.ebp      = 0,
			.ebx      = 0,
			.edx      = 0,
			.ecx      = 0,
			.eax      = 0,
			.intr     = 0,
			.error    = 0,
			.eip      = entrypoint,
			.cs       = 0x8,
			.eflags   = 0x202,
			};

			struct cpu_state* state = (void*) (stack - sizeof(new_state));
			*state=new_state;
			proc_new->main_state = state;

	} else { // Usermode
		pd_map_range(proc_new->pagedir,phy_base,vrt_base,PTE_WRITABLE|PTE_USER,size/PAGE_SIZE);
		proc_new->flags = 0;

		uint32_t *user_stack=malloc(PROC_STACK_SIZE) + PROC_STACK_SIZE;
		pd_map_range(proc_new->pagedir, vaddr2paddr(proc_new->pagedir, (vaddr_t) user_stack), STACK_HEAD - PROC_STACK_SIZE,PTE_WRITABLE | PTE_USER,PROC_STACK_SIZE);

		struct cpu_state new_state = {
			.gs       = 0x2b,
			.fs       = 0x2b,
			.es       = 0x2b,
			.ds       = 0x2b,
			.edi      = 0,
			.esi      = 0,
			.ebp      = 0,
			.ebx      = 0,
			.edx      = 0,
			.ecx      = 0,
			.eax      = 0,
			.intr     = 0,
			.error    = 0,
			.eip      = entrypoint,
			.cs       = 0x23,
			.eflags   = 0x202,
			.esp = STACK_HEAD,
			.ss       = 0x2b,
		};

		struct cpu_state* state = (void*) (stack - sizeof(new_state));
		*state=new_state;
		proc_new->main_state = state;
	 }


	 strncpy((char*)&proc_new->name       , name, 255);
	 strncpy((char*)&proc_new->description, desc, 255);
	 proc_new->port          = NULL;
	 proc_new->threads       = NULL;
	 proc_new->currentthread = NULL;
	 proc_new->proc_children = NULL;
	 proc_new->proc_parent   = currentprocess;
	 proc_new->tid_counter   = 0;
	 proc_new->freetid       = NULL;

	 while (lock) {}
	 lock=true;
	 asm volatile("cli");

	 proc_new->pid         = get_freepid();
	 proc_new->next        = proc0;
	 proc_new->prev        = proc0->prev;
	 proc0->prev           = proc_new;
	 proc_new->prev->next  = proc_new;
	 proc_new->flags      |= priority|ACTIV;

	 if (!currentprocess->proc_children) {
		currentprocess->proc_children = malloc(CHILD_STRUCT_SIZE);
		currentprocess->proc_children->prev = currentprocess->proc_children;
		currentprocess->proc_children->next = currentprocess->proc_children;
		currentprocess->proc_children->proc = proc_new;
		currentprocess->proc_children->return_value = 0;
		currentprocess->proc_children->pid = proc_new->pid;
	 } else {
		struct child *new_child = malloc(CHILD_STRUCT_SIZE);
		new_child->proc = proc_new;
		new_child->return_value = 0;
		new_child->pid = proc_new->pid;
		new_child->next = currentprocess->proc_children;
		new_child->prev = currentprocess->proc_children->prev;
		currentprocess->proc_children->prev->next = new_child;
		currentprocess->proc_children->prev = new_child;
	 }


	 asm volatile("sti");
	 lock=false;

	return proc_new->pid;
}

static void proc_structure_free(struct task_state *proc) {
	if (proc->port) {
		struct port_access *current = proc->port->next;
		struct port_access *next;
		while (current != proc->port) {
			next = current->next;
			free(current);
			current = next;
		}
		free(proc->port);
	}

	 if (proc->threads) {
		struct thread *current = proc->threads->next;
		struct thread *next;
		while (current != proc->threads) {
			next = current->next;
			free(current);
			current = next;
		}
		free(proc->threads);
	}

	if (proc->proc_children) {
		struct child *current = proc->proc_children->next;
		struct child *next;
		while (current != proc->proc_children) {
			next = current->next;
			free(current);
			current = next;
		}
		free(proc->proc_children);
	}

}

int proc_kill(struct task_state *proc) {
	if (proc) {
		while (lock) {}
		lock = true;
		asm volatile("cli");

		proc->prev->next = proc->next;
		proc->next->prev = proc->prev;
		struct child *temp = proc->proc_parent->proc_children;
		while (temp->proc !=proc) {
			temp->next;
		}
		temp->proc = NULL;

		if (!freepid) {
			freepid = malloc(12);
			freepid->prev = freepid;
			freepid->next = freepid;
			freepid->pid = proc->pid;
		} else {
			struct zombiepid *zombiepid_new = malloc(12);
			zombiepid_new->prev = freepid->prev;
			zombiepid_new->next = freepid;
			freepid->prev->next = zombiepid_new;
			freepid->prev = zombiepid_new;
		}
		proc_structure_free(proc);
		if (currentprocess != proc) {
			//TODO: Free allocated memory and pagdir
			free(currentprocess);
		} else {
			proc->flags |= TRASH;
		}
		asm volatile("sti");
		lock=false;
		return 0;
	}
	return 1;
}

void exit(int errorcode) {
	struct child *temp = currentprocess->proc_parent->proc_children;
	while (temp->proc !=currentprocess) {
		temp->next;
	}
	temp->return_value = errorcode;
	proc_kill(currentprocess);
	asm volatile("int $32");
}

struct task_state* proc_get(uint32_t pid) {
	if (proc0->pid == pid) {
		return proc0;
	}

	struct task_state* temp=proc0->next;
	while (temp->next!=proc0->next && temp->pid != pid ) {temp=temp->next;}
	if (temp->pid != pid) {
		return NULL;
	}
	return temp;
}



tid_t get_freetid(void) {
	tid_t new_tid;
	if (currentprocess->freetid) {
		new_tid = currentprocess->freetid->tid;
		if (currentprocess->freetid->next == currentprocess->freetid) {
			free(currentprocess->freetid);
			currentprocess->freetid=NULL;
		} else {
			currentprocess->freetid->prev->next = currentprocess->freetid->next;
			currentprocess->freetid->next->prev = currentprocess->freetid->prev;
			struct zombietid *freetid_new = currentprocess->freetid->next;
			free(currentprocess->freetid);
			currentprocess->freetid = freetid_new;
		}
	} else {
	   new_tid = currentprocess->tid_counter;
	   currentprocess->tid_counter++;
	}
	return new_tid;
}

uint32_t thread_create(uint32_t eip) {
	while (lock) {}
	lock=true;
	asm volatile("cli");

	if (!currentprocess->threads) {
		currentprocess->threads = malloc(THREAD_STRUCT_SIZE);
		currentprocess->threads->flags  = ACTIV|MAIN_THREAD;
		currentprocess->threads->thread_state = currentprocess->main_state;
		currentprocess->threads->next = currentprocess->threads;
		currentprocess->threads->prev = currentprocess->threads;
		currentprocess->threads->tid = get_freetid();
		currentprocess->currentthread = currentprocess->threads;
	}

	struct thread * new_thread = malloc(THREAD_STRUCT_SIZE);
	new_thread->next = currentprocess->threads;
	new_thread->prev = currentprocess->threads->prev;
	currentprocess->threads->prev->next = new_thread;
	currentprocess->threads->prev = new_thread;
	new_thread->flags = ACTIV;
	new_thread->tid = get_freetid();

	uint32_t* stack=malloc(PROC_STACK_SIZE) + PROC_STACK_SIZE;
	if (currentprocess->flags & KERNELMODE) {
		struct cpu_state new_state = {
			.gs       = 0x10,
			.fs       = 0x10,
			.es       = 0x10,
			.ds       = 0x10,
			.edi      = 0,
			.esi      = 0,
			.ebp      = 0,
			.ebx      = 0,
			.edx      = 0,
			.ecx      = 0,
			.eax      = 0,
			.intr     = 0,
			.error    = 0,
			.eip      = eip,
			.cs       = 0x8,
			.eflags   = 0x202,
		};

		struct cpu_state* state = (void*) (stack - sizeof(new_state));
		*state=new_state;
		new_thread->thread_state = state;
	} else {
		uint32_t *user_stack=malloc(PROC_STACK_SIZE)+PROC_STACK_SIZE;
		pd_map_range(currentprocess->pagedir,user_stack,STACK_HEAD-PROC_STACK_SIZE,PTE_WRITABLE|PTE_USER,PROC_STACK_SIZE);

		struct cpu_state new_state = {
			.gs       = 0x2b,
			.fs       = 0x2b,
			.es       = 0x2b,
			.ds       = 0x2b,
			.edi      = 0,
			.esi      = 0,
			.ebp      = 0,
			.ebx      = 0,
			.edx      = 0,
			.ecx      = 0,
			.eax      = 0,
			.intr     = 0,
			.error    = 0,
			.eip      = eip,
			.cs       = 0x23,
			.eflags   = 0x202,
			.esp = STACK_HEAD,
			.ss       = 0x2b,
		};

		struct cpu_state* state = (void*) (stack - sizeof(new_state));
		*state = new_state;
		new_thread->thread_state = state;
	}

	asm volatile("sti");
	lock = false;
	return new_thread->tid;
}

int thread_kill(struct thread *thr) {
	if (thr) {
		while (lock) {}
		lock = true;
		asm volatile("cli");

		thr->flags = ZOMBIE;

		asm volatile("sti");
		lock = false;

		return 0;
	} else {
		return 1;
	}
}

void thread_exit(int errorcode) {
	currentprocess->currentthread->return_value = errorcode;
	thread_kill(currentprocess->currentthread);
	asm volatile("int $32");
}

struct thread *thread_get(tid_t tid) {
	if (tid) {
		struct thread *temp = currentprocess->threads->next;
		while (temp->next != currentprocess->threads->next && temp->tid != tid) { temp = temp->next; }
		if (temp->tid != tid) {
			return NULL;
		}
		return temp;
	} else {
		return NULL;
	}
}

/*int get_return_value(tid_t tid) {
TODO: Search ZOMBIE, return value, free struct and tid

}*/
