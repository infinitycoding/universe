#ifndef _arch_paging_h_
#define _arch_paging_h_

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
	@author Tom Slawik <tom.slawik@gmail.com>
	@author Michael Sippel (Universe Team) <micha.linuxfreak@gmail.com>
*/

#include <stdint.h>
#include <multiboot.h>
#include <cpu.h>

#define PD_LENGTH 1024
#define PT_LENGTH 1024

#define PAGE_FRAME_NUMBER(x) ((x) >> 12)
#define PAGE_FRAME_ADDR(x) ((x) << 12)

#define NUM_PAGES(n) ((((n) + 0xfff) & 0xfffff000) / PAGE_SIZE)

#define PDE_INDEX(x) ((x) >> 22)
#define PTE_INDEX(x) ((x) >> 12 & 0x03FF)

#define VMM_PRESENT  0x1
#define VMM_WRITABLE 0x2
#define VMM_USER     0x4

typedef uint32_t pde_t;
typedef uint32_t pte_t;
typedef pte_t* pt_t;

typedef struct {
	pde_t *entries;
	paddr_t phys_addr;
} arch_vmm_context_t;


void ARCH_INIT_PAGING(struct multiboot_struct *mb_info);
void arch_vmm_create_context(arch_vmm_context_t *context);
void arch_vmm_destroy_context(arch_vmm_context_t *context);
void arch_sync_pts(arch_vmm_context_t *src, arch_vmm_context_t *dest, int index_low, int index_high);

pt_t pt_get(arch_vmm_context_t *context, int index, uint8_t flags);
pt_t pt_create(arch_vmm_context_t *context, int index, uint8_t flags);
void pt_destroy(arch_vmm_context_t *context, int index);
int arch_map(arch_vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags);
int arch_unmap(arch_vmm_context_t *context, vaddr_t frame);
vaddr_t arch_vaddr_find(arch_vmm_context_t *context, int num, vaddr_t limit_low, vaddr_t limit_high, int flags);
paddr_t arch_vaddr2paddr(arch_vmm_context_t *context, vaddr_t vaddr);
void arch_switch_context(arch_vmm_context_t *context);

void pagefault_handler(struct cpu_state **cpu_p);

#endif

