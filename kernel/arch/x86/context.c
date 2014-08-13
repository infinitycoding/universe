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
#include <cpu.h>
#include <pmm.h>
#include <string.h>
#include <memory_layout.h>
#include <mm/paging.h>
#include <mm/heap.h>


struct cpu_state *arch_create_thread_context(struct arch_thread_context *context, privilege_t prev, vaddr_t entry, vaddr_t return_address, int argc, void **argv)
{
    void *kernel_stack = malloc(0x1000);
    struct cpu_state *new_state = kernel_stack + 0x1000 - sizeof(struct cpu_state) - 3*sizeof(uint32_t);
    context->state = new_state;

    memset(new_state, 0, sizeof(struct cpu_state));
    new_state->eip = entry;
    new_state->eflags = 0x202;

    uint32_t *stack;
    if(prev == KERNELMODE)
    {
        new_state->cs = 0x08;
        new_state->ds = 0x10;
        new_state->es = 0x10;
        new_state->fs = 0x10;
        new_state->gs = 0x10;
        stack = kernel_stack + 0x1000 - 3*sizeof(uint32_t);
        *--stack = (uint32_t) argv;
        *--stack = argc;
        *--stack = (uint32_t) return_address;
    }
    else
    {
        paddr_t pframe = pmm_alloc_page();
        vmm_map(&context->memory, pframe, MEMORY_LAYOUT_STACK_TOP-0x1000, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
        new_state->esp = (uint32_t) MEMORY_LAYOUT_STACK_TOP - 3*sizeof(uint32_t);

        vaddr_t vframe = vmm_automap_kernel(current_context, pframe, VMM_PRESENT | VMM_WRITABLE);

        stack = (uint32_t *) vframe + 0xFFF - 3*sizeof(uint32_t);
        printf("v frame: %x    stack:%x\n", vframe, stack);
        *--stack = (uint32_t) argv;
        *--stack = argc;
        *--stack = (uint32_t) return_address;

        vmm_unmap(current_context, vframe);

        new_state->cs = 0x1b;
        new_state->ss = 0x23;
    }

    return new_state;

}

void arch_destroy_thread_context(struct arch_thread_context *context)
{
    free(context->state);
    arch_vmm_destroy_context(&context->memory.arch_context);
}
