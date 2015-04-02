/*
     Copyright 2012-2015 Infinitycoding all rights reserved
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
 *	@author Michael Sippel <micha@infinitycoding.de>
 */

#include <cpu.h>
#include <gdt.h>
#include <io.h>
#include <sched/thread.h>

/**
 * @brief			performs context switches
 * @param thread	pointer to the thread state to switch to
 * @return			new cpu state
 */
struct cpu_state *dispatch_thread(struct thread_state *thread)
{
    struct cpu_state *cpu = thread->context.state;
    set_kernelstack(cpu+1);
    vmm_switch_context(&thread->context.memory);
    set_iobmp(&thread->context);
    return cpu;
}

