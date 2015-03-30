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

#include <sched/thread.h>
#include <sched/syscalls.h>
#include <event/time.h>
#include <syscall.h>
#include <mm/shm.h>
#include <printf.h>


#define OS_VERSION 1

extern struct thread_state *current_thread;
extern struct process_state *kernel_state;



void usys_identify_os(struct cpu_state **cpu)
{
    (*cpu)->CPU_ARG0 = 'u' | ('n' << 8) | ('i' << 16) | (OS_VERSION << 24);
};



#define DEFINED_LINUX_FUNCTIONS 60


void (*linux_functions[])(struct cpu_state **cpu) =
{
    NULL, sys_exit, sys_fork, sys_waitpid,
    sys_brk, sys_shm_get, sys_shm_ctl, sys_shm_attach, sys_shm_detach, sys_pause, sys_kill, sys_raise, sys_signal
};

void linux_syscall_handler(struct cpu_state **cpu)
{
    if( (*cpu)->CPU_ARG0 < DEFINED_LINUX_FUNCTIONS)
    {
        if(linux_functions[(*cpu)->CPU_ARG0] != NULL)
            linux_functions[(*cpu)->CPU_ARG0](cpu);
    }
}


#define DEFINED_UNIVERSE_FUNCTIONS 8
void (*universe_functions[])(struct cpu_state **cpu) =
{
    usys_thread_exit,usys_thread_launch,alloc_memory,usys_identify_os
};


void universe_syscall_handler(struct cpu_state **cpu)
{
    if( (*cpu)->CPU_ARG0 < DEFINED_UNIVERSE_FUNCTIONS)
    {
        if(universe_functions[(*cpu)->CPU_ARG0] != NULL)
            universe_functions[(*cpu)->CPU_ARG0](cpu);
    }
}
