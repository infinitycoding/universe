#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

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
#include <cpu.h>
#include <mm/vmm.h>
#include <sched/thread.h>




//Definitions
#define STACK_HEAD 0xBFFFFFFF
#define KERNEL_STACK_SIZE 4096

#define PORT_ACCESS_STRUCT_SIZE 10
#define THREAD_STRUCT_SIZE 24
#define CHILD_STRUCT_SIZE 20
#define ZOMBIEPID_STRUCT_SIZE 12
#define CPU_STATE_STRUCT_SIZE 76




void INIT_SCHEDULER(void);
struct cpu_state *task_schedule(struct cpu_state *cpu);


#endif
