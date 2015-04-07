/*
     Copyright 2015 Infinitycoding all rights reserved
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
 *  @author Michael Sippel <micha@infinitycoding.de>
 */

#include <sched/process.h>
#include <sched/thread.h>
#include <list.h>
#include <mm/heap.h>
#include <stddef.h>

extern struct thread_state *current_thread;

signal_t *get_signal(struct process_state *process, unsigned int id)
{
    return (signal_t*) list_get_by_int(process->signals, offsetof(signal_t, id), id);
}

void send_signal(struct process_state *process, unsigned int id)
{
    signal_t *signal = get_signal(process, id);

    if(signal == NULL)
    {
        return;
    }

    switch(signal->action)
    {
        case NONE:
            break;

            process_wakeup(process);

        case WAKEUP:

            break;
        case HANDLER:
            // TODO
            break;

        case THREAD:
            break;
    }
}

void sys_pause(struct cpu_state **cpu)
{
    thread_suspend(current_thread);
    (*cpu)->CPU_ARG0 = 0;

    *cpu = (struct cpu_state *)task_schedule(*cpu);
}

void sys_raise(struct cpu_state **cpu)
{
    unsigned int sig = (*cpu)->CPU_ARG1;

    send_signal(current_thread->process, sig);
    (*cpu)->CPU_ARG0 = 0;
}

void sys_signal(struct cpu_state **cpu)
{
    unsigned int id = (*cpu)->CPU_ARG1;
    enum signal_action action = (*cpu)->CPU_ARG2;
    void (*handler)(int pid) = (*cpu)->CPU_ARG3;

    signal_t *signal = get_signal(current_thread->process, id);

    if(signal == NULL)
    {
        signal = (signal_t*) malloc(sizeof(signal_t));
        list_push_back(current_thread->process->signals, signal);
    }

    if(action == NONE)
    {
        // TODO: remove
    }

    signal->id = id;
    signal->action = action;
    signal->handler = handler;

    (*cpu)->CPU_ARG0 = 0;
}

