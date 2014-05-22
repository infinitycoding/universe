#ifndef _PAGING_H_
#define _PAGING_H_

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
 *  @author Michael Sippel (Universe Team) <michamimosa@gmail.com>
 */

#include <sys/multiboot.h>
#include <arch_paging.h>
#include <stdint.h>
#include <cpu.h>






typedef struct vmm_context
{
    arch_vmm_context_t arch_context;
} vmm_context_t;

extern vmm_context_t *current_context;

// arch
void ARCH_INIT_PAGING(struct multiboot_struct *mb_info);
arch_vmm_context_t *arch_create_vmm_context(void);
void arch_destroy_vmm_context(arch_vmm_context_t *context);
void arch_switch_context(arch_vmm_context_t *context);
void arch_update_context(arch_vmm_context_t *context);
int arch_map(arch_vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags);
int arch_unmap(arch_vmm_context_t *context, vaddr_t frame);
vaddr_t arch_vaddr_find(arch_vmm_context_t *context, int num, vaddr_t limit_low, vaddr_t limit_high, int flags);
paddr_t arch_vaddr2paddr(arch_vmm_context_t *context, vaddr_t vaddr);
void arch_sync_pts(arch_vmm_context_t *src, arch_vmm_context_t *dest, int index_low, int index_high);
void arch_fork_context(arch_vmm_context_t *src, arch_vmm_context_t *dest);
void page_fault_handler(struct cpu_state **cpu_p);

// global
void INIT_PAGING(struct multiboot_struct *mb_info);
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

#endif

