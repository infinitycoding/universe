#ifndef _thread_h_
#define _thread_h_
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
#include <process.h>
#include <list.h>
#include <paging.h>
#define THREAD_ACTIV 1
#define THREAD_FREEZED 2
#define THREAD_KERNELMODE 4
#define THREAD_ZOMBIE 8
#define THREAD_STACK_SIZE 4096
#define THREAD_WAITPID 16



struct thread_state
{
    struct cpu_state *state;
    struct process_state *process;
    vmm_context_t context;
    uint32_t ticks;
    uint16_t flags;
    uint32_t waitpid;
    int return_value;
    tid_t tid; //Thread ID
};

struct thread_state *thread_create(struct process_state *process, privilege_t prev,uintptr_t eip, struct cpu_state *state, int argc, void **argv, void *return_address, vmm_context_t *context);
void thread_kill(struct thread_state *thread);
void thread_kill_sub(struct thread_state *thread);
void thread_exit(struct cpu_state **cpu);
void launch_thread(struct cpu_state **cpu);
#endif
