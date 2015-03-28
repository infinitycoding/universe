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
 * @file /arch/x86/mm/pagetable.c
 * @brief x86-architecture dependent paging functions
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <arch/mm/pmm.h>
#include <arch/mm/layout.h>
#include <mm/vmm.h>

// some macros
#define PT_PADDR(i) (context->entries[i] & ~0xFFF)
#define PT_VADDR(i) (MEMORY_LAYOUT_PAGING_STRUCTURES_START + PT_LENGTH*sizeof(pte_t)*i)

extern vmm_context_t *current_context;


/**
 * @fn arch_pt_get
 * @brief Get the pagetable at index
 *
 * @param pd pagedirectory
 * @param index index
 * @param flags flags
 * @return pagetable
 */
pt_t arch_pt_get(arch_vmm_context_t *context, int index)
{
    pt_t pt;

    if(current_context != NULL)
    {
        if(context == arch_current_context)
        {
            pt = (pt_t) PT_VADDR(index);
        }
        else
        {
            pt = (pt_t) PT_PADDR(index);
            pt = (pt_t) vmm_automap_kernel(current_context, (paddr_t) pt, VMM_PRESENT | VMM_WRITABLE);
        }
    }
    else
    {
        pt = (pt_t) PT_PADDR(index) + MEMORY_LAYOUT_KERNEL_START;
    }

    return pt;
}

/**
 * @fn arch_pt_create
 * @brief Create a new pagetable
 *
 * @param context pagedirectory
 * @param index index
 * @param flags flags
 * @return pagetable
 */
pt_t arch_pt_create(arch_vmm_context_t *context, int index, uint8_t flags)
{
    pt_t pt;
    if(current_context == NULL)
        pt = (pt_t) pmm_alloc_page_limit(0);
    else
        pt = (pt_t)pmm_alloc_page();

    context->entries[index] = (pde_t) pt | flags | VMM_PRESENT;

    pt = arch_pt_get(context, index);
    memset(pt, 0, 4096);

    return pt;
}

/**
 * @fn arch_pt_destroy
 * @brief Destroy a pagetable
 *
 * @param context pagedirectory
 * @param index index
 * @return void
 */
void arch_pt_destroy(arch_vmm_context_t *context, int index)
{
    pmm_mark_page_as_free((paddr_t)context->entries[index]);
    context->entries[index] = 0;
}


