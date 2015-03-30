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
 * @file /arch/x86/mm/context.c
 * @brief x86-architecture dependent paging functions
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */

#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/layout.h>

#include <mm/vmm.h>

extern vmm_context_t *current_context;

/**
 * @fn arch_vmm_create_context
 * @brief Initalizes a new vmm context from a pointer.
 * The memory must be allocated before.
 *
 * @param context pointer to the memory structure
 * @return void
 */
void vmm_create_context(vmm_context_t *context)
{
    uintptr_t paddr = (uintptr_t) pmm_alloc_page();
    uintptr_t vaddr = vmm_automap_kernel(current_context, paddr, VMM_PRESENT | VMM_WRITABLE);
    memset((void*)vaddr, 0, PAGE_SIZE);

    context->entries = (pde_t *) vaddr;
    context->phys_addr = paddr;

    arch_vmm_map_context(context);
}

/**
 * @fn vmm_destroy_context
 * @brief Destroys a vmm context
 *
 * @param context pointer to to destroying context
 * @return void
 */
void vmm_destroy_context(vmm_context_t *context)
{
    int pt;
    for (pt = 0; pt < PD_LENGTH; ++pt)
    {
        if (context->entries[pt] & VMM_PRESENT)
        {
            pmm_mark_page_as_free((paddr_t)context->entries[pt]);
        }
    }
    pmm_mark_page_as_free((paddr_t)context->phys_addr);
}

/**
 * @fn arch_fork_context
 * @brief create a copy of a context and its mappings
 *
 * @param src the original context
 * @param dest the new context
 * @return void
 */
void vmm_fork_context(vmm_context_t *src, vmm_context_t *dest)
{
    if(src != NULL && dest != NULL)
    {
        if(src->entries != NULL && dest->entries != NULL)
        {
            int i;
            for(i = 0; i < 1024; i++)
            {
                if(src->entries[i] & VMM_PRESENT)
                {
                    int flags = src->entries[i] & 0xfff;

                    pt_t *pt_src = (pt_t *) pt_get(src, i);
                    pt_t *pt_dest = (pt_t *) pt_create(dest, i, flags);

                    if(pt_src == NULL || pt_dest == NULL)
                    {
                        printf("aaaah");
                        while(1);
                    }

                    memcpy(pt_dest, pt_src, 0x1000);

                    if(src != current_context)
                        vmm_unmap(current_context, (vaddr_t) pt_src);

                    vmm_unmap(current_context, (vaddr_t) pt_dest);
                }
            }
        }
    }
}

/**
 * @fn arch_update_context
 * @brief syncronize the kernelspace
 *
 * @param context context
 * @return void
 */
void arch_vmm_map_context(vmm_context_t *context)
{
    vmm_sync_context(context, current_context, MEMORY_LAYOUT_KERNEL_START, MEMORY_LAYOUT_KERNEL_END);
    context->entries[PDE_INDEX(MEMORY_LAYOUT_PAGING_STRUCTURES_START)] = (uint32_t) context->phys_addr | VMM_PRESENT | VMM_WRITABLE;
}

/**
 * @fn arch_sync_pts
 * @brief Copy pagetables
 *
 * @param dest destination context
 * @param src source context
 * @param index_low start pde-index
 * @param index_high end pde-index
 * @return void
 */
void vmm_sync_context(vmm_context_t *dest, vmm_context_t *src, vaddr_t limit_low, vaddr_t limit_high)
{
    int index_low = PDE_INDEX(limit_low);
    int index_high = PDE_INDEX(limit_high);

    int i;
    for(i = index_low; i <= index_high; i++)
    {
        dest->entries[i] = src->entries[i];
    }
}


/**
 * @fn arch_switch_context
 * @brief Switch the pagedirectory
 *
 * @param context pagedirectory
 * @param flags flags
 * @return void
 */
void arch_vmm_switch_context(vmm_context_t *context)
{
    asm volatile ("mov %0, %%cr3" : : "r" (context->phys_addr));
}


