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

//Flags for Threads and Procs
#define ACTIV 1
#define FREEZED 2
#define KERNELMODE 4
#define NORMAL_PRIORITY 8
#define REALTIME_PRIORITY 16
#define TRASH 32

//just for threads
#define MAIN_THREAD 64
#define ZOMBIE 128

#define FREQ_REALTIME 18
#define FREQ_NORMAL 1000
#define FREQ_BACKGRUND 10000

//Definitions
#define STACK_HEAD 0xBFFFFFFF
#define KERNEL_STACK_SIZE 4096
#define PROC_STACK_SIZE 4096
#define PORT_ACCESS_STRUCT_SIZE 10
#define THREAD_STRUCT_SIZE 24
#define CHILD_STRUCT_SIZE 20
#define ZOMBIEPID_STRUCT_SIZE 12
#define CPU_STATE_STRUCT_SIZE 76



typedef uint32_t pid_t;
typedef uint32_t tid_t;

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
    struct cpu_state *thread_state;
    uint32_t flags;
    int return_value;
    tid_t tid; //Thread ID
};

struct child
{
    struct child *prev;
    struct child *next;
    int return_value;
    pid_t pid;
    struct task_state *proc;
};


struct zombiepid
{
    struct zombiepid *prev;
    struct zombiepid *next;
    uint32_t pid;
};

struct zombietid
{
    struct zombietid *prev;
    struct zombietid *next;
    tid_t tid;
};

#define TASK_STATE_STRUCT_SIZE 564
struct task_state
{
    struct task_state *prev;
	struct task_state *next;
	pid_t pid;
    struct cpu_state *main_state;
    pd_t *pagedir;
    char name[256];
	char description[256];
    struct port_access *port;
    struct task_state* proc_parent;
    struct thread *threads;
    struct thread *currentthread;
    uint32_t tid_counter;
    struct zombietid *freetid;
    struct child *proc_children;
    uint32_t flags;

};


void INIT_SCEDULER(void);
struct cpu_state *task_schedul(struct cpu_state *cpu);
pid_t proc_create(prev_t prev,vaddr_t vrt_base,paddr_t phy_base,size_t size,vaddr_t entrypoint,char* name,char* desc,priority_t priority) ;
int proc_kill(struct task_state *proc);
void exit(int);
struct task_state* proc_get(uint32_t pid);
uint32_t thread_create(uint32_t eip);
int thread_kill(struct thread *thr);
void thread_exit(int errorcode);
struct thread *thread_get(tid_t tid);




#endif
