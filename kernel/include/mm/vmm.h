#ifndef _VMM_H_
#define _VMM_H_

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
 * @file /include/mm/vmm.h
 * @brief header file for common paging functions
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */
#include <arch.h>

#ifdef _VMM_

#include <multiboot.h>
#include <arch/mm/vmm.h>
#include <stdint.h>
#include <cpu.h>

/**
 * @struct vmm_context
 * @brief Abstracts an architecture dependend vmm-context.
 */
typedef struct vmm_context
{
    arch_vmm_context_t arch_context;
} vmm_context_t;

extern vmm_context_t *current_context; /// defined in mm/paging.c

// arch must have
void ARCH_INIT_VMM(struct multiboot_struct *mb_info);

void arch_vmm_create_context(arch_vmm_context_t *context);
void arch_vmm_fork_context(arch_vmm_context_t *src, arch_vmm_context_t *dest);
void arch_vmm_destroy_context(arch_vmm_context_t *context);
void arch_vmm_sync_context(arch_vmm_context_t *dest, arch_vmm_context_t *src, vaddr_t limit_low, vaddr_t limit_high);
void arch_vmm_switch_context(arch_vmm_context_t *context);

int arch_vmm_map(arch_vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags);
int arch_vmm_unmap(arch_vmm_context_t *context, vaddr_t frame);
int arch_vmm_is_present(arch_vmm_context_t *context, vaddr_t vaddr);

vaddr_t arch_vaddr_find(arch_vmm_context_t *context, int num, vaddr_t limit_low, vaddr_t limit_high);
paddr_t arch_vaddr2paddr(arch_vmm_context_t *context, vaddr_t vaddr);

void pagefault_handler(struct cpu_state **cpu_p);


// global
void INIT_VMM(struct multiboot_struct *mb_info);
vmm_context_t *create_vmm_context(void);
void destroy_vmm_context(vmm_context_t *context);
int vmm_map(vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags);
int vmm_unmap(vmm_context_t *context, vaddr_t frame);
int vmm_map_range(vmm_context_t *context, paddr_t pframe, vaddr_t vframe, int pages, uint8_t flags);
int vmm_unmap_range(vmm_context_t *context, vaddr_t frame, int pages);
vaddr_t vmm_automap_kernel(vmm_context_t *context, paddr_t pframe, uint8_t flags);
vaddr_t vmm_automap_user(vmm_context_t *context, paddr_t pframe, uint8_t flags);
vaddr_t vmm_automap_kernel_range(vmm_context_t *context, paddr_t pframe, int pages, uint8_t flags);
vaddr_t vmm_automap_user_range(vmm_context_t *context, paddr_t pframe, int pages, uint8_t flags);
void vmm_switch_context(vmm_context_t *context);
void vmm_destroy_context(vmm_context_t *context);
void vmm_create_context(vmm_context_t *context);

void alloc_memory(struct cpu_state **cpu);
void sys_brk(struct cpu_state **cpu);

#endif

#endif

