#ifndef _THREAD_H_
#define _THREAD_H_
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
 */

#include <arch_context.h>
#include <sched/process.h>
#include <mm/paging.h>
#include <stdint.h>
#include <list.h>

#define THREAD_ACTIV 1
#define THREAD_FREEZED 2
#define THREAD_KERNELMODE 4
#define THREAD_ZOMBIE 8
#define THREAD_STACK_SIZE 4096
#define THREAD_WAITPID 16
#define THREAD_TRIGGER 32



struct thread_state
{
    struct arch_thread_context context;
    struct process_state *process;
    uint32_t ticks;
    uint16_t flags;
    uint32_t waitpid;
    int return_value;
    tid_t tid; //Thread ID
};

struct thread_state *thread_create(struct process_state *process, privilege_t prev, uint32_t eip, int argc, void **argv, char **environ, void *return_address, vmm_context_t *context);
void thread_kill(struct thread_state *thread);
void thread_kill_sub(struct thread_state *thread);
void thread_exit(struct cpu_state **cpu);
void launch_thread(struct cpu_state **cpu);
struct thread_state *kernel_thread_create(uintptr_t eip, int argc, void **argv, char **environ);
void kernel_thread_exit(void);
void thread_start(struct thread_state *thread);

struct thread_state *thread_clone(struct process_state *process, struct thread_state *src_thread);
#endif
