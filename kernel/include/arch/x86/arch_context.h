#ifndef _ARCH_CONTEXT_H_
#define _ARCH_CONTEXT_H_

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
#include <mm/paging.h>

struct arch_thread_context
{
    struct cpu_state *state;
    vaddr_t kernel_mode_stack;
    vaddr_t programm_stack;
    vmm_context_t memory;
}arch_thread_context;

struct cpu_state *arch_create_thread_context(struct arch_thread_context *context, privilege_t prev, vaddr_t entry, vaddr_t return_address, int argc, void **argv);
void arch_destroy_thread_context(struct arch_thread_context *context);
#endif