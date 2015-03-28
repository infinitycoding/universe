#ifndef _ARCH_PAGING_H_
#define _ARCH_PAGING_H_

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
 * @file /include/arch/x86/arch_paging.h
 * @brief header file for optional x86-architecture dependent paging
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <stdint.h>
#include <cpu.h>
#include <pmm.h>
#include <multiboot.h>


#define PD_LENGTH 1024 /// length of a page-directory
#define PT_LENGTH 1024 /// length of a page-table

#define PAGE_FRAME_NUMBER(x) ((x) >> 12) /// get the page index from a adress
#define PAGE_FRAME_ADDR(x) ((x) << 12)   /// generate an adress from page index

#define NUM_PAGES(n) ((((n) + 0xfff) & 0xfffff000) / PAGE_SIZE) /// calculate the number of pages used by n bytes

#define PDE_INDEX(x) ((x) >> 22)          /// get the pagedirectory index from an adress
#define PTE_INDEX(x) ((x) >> 12 & 0x03FF) /// get the pagetable index from an adress

#define arch_current_context &current_context->arch_context

// paging flags
#define VMM_PRESENT  0x1
#define VMM_WRITABLE 0x2
#define VMM_USER     0x4

typedef uint32_t pde_t;
typedef uint32_t pte_t;
typedef pte_t* pt_t;

/**
 * @struct arch_vmm_context
 * @brief the x86-dependent vmm context struct
 */
typedef struct arch_vmm_context
{
    pde_t *entries;
    paddr_t phys_addr;
} arch_vmm_context_t;

void ARCH_INIT_VMM(struct multiboot_struct *mb_info);

// context
void arch_vmm_create_context(arch_vmm_context_t *context);
void arch_vmm_fork_context(arch_vmm_context_t *src, arch_vmm_context_t *dest);
void arch_vmm_destroy_context(arch_vmm_context_t *context);
void arch_vmm_update_context(arch_vmm_context_t *context);
void arch_vmm_sync_pts(arch_vmm_context_t *dest, arch_vmm_context_t *src, int index_low, int index_high);
void arch_vmm_switch_context(arch_vmm_context_t *context);

// pagetable
pt_t arch_pt_get(arch_vmm_context_t *context, int index);
pt_t arch_pt_create(arch_vmm_context_t *context, int index, uint8_t flags);
void arch_pt_destroy(arch_vmm_context_t *context, int index);

// pages
int arch_vmm_map(arch_vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags);
int arch_vmm_unmap(arch_vmm_context_t *context, vaddr_t frame);
int arch_vmm_is_present(arch_vmm_context_t *context, vaddr_t vaddr);

vaddr_t arch_vaddr_find(arch_vmm_context_t *context, int num, vaddr_t limit_low, vaddr_t limit_high);
paddr_t arch_vaddr2paddr(arch_vmm_context_t *context, vaddr_t vaddr);

void pagefault_handler(struct cpu_state **cpu_p);

#endif

