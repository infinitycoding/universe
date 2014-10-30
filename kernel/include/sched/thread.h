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

#define THREAD_ACTIVE 1
#define THREAD_FREEZED 2
#define THREAD_KERNELMODE 4
#define THREAD_ZOMBIE 8
#define THREAD_STACK_SIZE 4096
#define THREAD_WAITPID 16
#define THREAD_TRIGGER 32

struct thread_state
{
    struct process_state *process;
    tid_t tid;
    uint32_t waitpid;

    uint32_t ticks;
    uint16_t flags;

    int return_value;
    struct arch_thread_context context;

    // i put that here.. where else?
    int getdents_pos;
    int getdents_old_fd;
};

struct thread_state *thread_create(struct process_state *process, privilege_t prev, vaddr_t eip, char **argv, char **environ, vaddr_t return_address, vmm_context_t *context);
struct thread_state *kernel_thread_create(int (*thread)(int argc, char **argv, char **environ), char **argv, char **environ);
struct thread_state *thread_clone(struct process_state *process, struct thread_state *src_thread);

void thread_sync_context(struct thread_state *thread);

void thread_kill(struct thread_state *thread);
void thread_kill_sub(struct thread_state *thread);
void thread_exit(struct cpu_state **cpu);
void kernel_thread_exit(void);

void launch_thread(struct cpu_state **cpu);
void thread_start(struct thread_state *thread);

void thread_suspend(struct thread_state *thread);
void thread_wakeup(struct thread_state *thread);

#endif

