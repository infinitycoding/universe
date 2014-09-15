#ifndef _TRIGGER_H_
#define _TRIGGER_H_

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

#include <stdint.h>
#include <sched/process.h>
#include <sched/thread.h>

typedef enum
{
    WAIT_PID =    0,
    WAIT_TID =    1,
    WAIT_EVENT =  2,
    WAIT_TIME =   3,
    WAIT_PIPE =   4,
    WAIT_INT =    5,
    WAIT_EXC =    6,
} trigger_t;

struct trigger_entry
{
    uint32_t type;
    uint32_t ID; // event ID or the parameter of waitpid
    bool proc;
    void *object; // pointer to the thread or Process struct. Depends on bool proc
    void (*callback)(struct cpu_state **cpu);
};


void INIT_TRIGGER(void);
void suspend_thread(struct thread_state *object);
void suspend_process(struct process_state *object);
void wakeup_thread(struct thread_state *object);
void wakeup_process(struct process_state *object);
int remove_event_trigger(void *object, uint32_t ID);
int remove_event(uint32_t ID);
int send_event(uint32_t ID);
uint32_t get_new_event_ID(void);
uint32_t add_event_trigger(bool proc, void *object, void (*callback)(struct cpu_state **cpu));
void add_trigger(trigger_t type, uint32_t ID, bool proc, void *object, void (*callback)(struct cpu_state **cpu));
void send_killed_process(struct process_state *proc);
int handle_interupts(struct cpu_state **cpu);
int add_int_trigger(int irq, struct thread_state *object,void (*callback)(int irq));

#endif
