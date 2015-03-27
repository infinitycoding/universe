#ifndef _SCHED_SYSCALLS_H_
#define _SCHED_SYSCALLS_H_

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

#include <cpu.h>

void usys_thread_exit(struct cpu_state **cpu);
void usys_thread_launch(struct cpu_state **cpu);

void sys_exit(struct cpu_state **cpu);
void sys_fork(struct cpu_state **cpu);
void sys_waitpid(struct cpu_state **cpu);
void sys_getpid(struct cpu_state **cpu);
void sys_execve(struct cpu_state **cpu);

void sys_pause(struct cpu_state **cpu);
void sys_kill(struct cpu_state **cpu);
void sys_raise(struct cpu_state **cpu);
void sys_signal(struct cpu_state **cpu);

#endif

