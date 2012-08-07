#ifndef _scheduler_h_
#define _scheduler_h_
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


#include <stdint.h>
#include <cpu.h>
#include <paging.h>

//Flags
#define ACTIV 1
#define FREEZED 2
#define KERNELMODE 4
#define NORMAL_PRIORITY 8
#define REALTIME_PRIORITY 16
//to be continued

#define FREQ_REALTIME 18
#define FREQ_NORMAL 1000
#define FREQ_BACKGRUND 10000

#define STACK_HEAD 0xBFFFFFFF

typedef uint32_t pid_t;

typedef enum{
    background = 0,
    normal     = NORMAL_PRIORITY,
    realtime   = REALTIME_PRIORITY,

}priority_t;

typedef enum{
    user_mode =0,
	kernel_mode = 1,
}prev_t;

struct port_access
{
    struct port_access *prev;
    struct port_access *next;
    uint16_t portnum;
};

struct thread
{
    struct thread *prev;
    struct thread *next;
    struct cpu_state thred_state;
    uint32_t flags;
};

struct child
{
    struct child *prev;
    struct child *next;
    struct task_state *proc;
};

struct zombepid
{
    struct zombepid *prev;
    struct zombepid *next;
    uint32_t pid;
};



//size: 628B
struct task_state
{
    struct task_state *prev;
	struct task_state *next;
	pid_t pid;
    struct cpu_state main_state;
    pd_t *pagedir;
    char name[256];
	char description[256];
    struct port_access *port;
    struct task_state* proc_parent;
    struct thread *threads;
    struct thread *currentthread;
    struct child *proc_children;
    uint32_t flags;

};


void INIT_SCEDULER(void);
struct cpu_state *task_schedul(struct cpu_state *cpu);
pid_t proc_create(prev_t prev,vaddr_t vrt_base,paddr_t phy_base,size_t size,vaddr_t entrypoint,char* name,char* desc,priority_t priority) ;
void proc_kill(struct task_state *proc);



#endif
