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

/**

  @author Simon Diepold aka. Tdotu (Universe Team) <simon.diepold@infinitycoding.de>

 */

#include <Syscall.h>
#include <thread.h>
#include <scheduler.h>

extern struct thread_state *current_thread;
extern struct process_state *kernel_state;

/**
    Todo: create a call table and write a bunch of functions...

 **/

void syscall_handler(struct cpu_state **cpu)
{
    struct cpu_state *state = *cpu;

    switch(state->eax)
    {
        case 0:
            printf("%s",state->ebx);
        break;

        case 1:
            current_thread->flags |= THREAD_ZOMBIE;
            *cpu = task_schedule(*cpu);
        break;



    }

}
