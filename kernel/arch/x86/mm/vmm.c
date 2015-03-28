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
 * @file /arch/x86/vmm.c
 * @brief x86-architecture dependent paging functions
 *
 * @author Michael Sippel <micha@infinitycoding.de>
 */


#include <printf.h>
#include <string.h>
#include <panic.h>

#include <memory_layout.h>
#include <mm/paging.h>

#include <sched/thread.h>

#include <cpu.h>
#include <idt.h>
#include <pmm.h>


static inline void arch_flush_tlb(vaddr_t addr);

extern struct thread_state *current_thread;

#define PT_PADDR(i) (context->entries[i] & ~0xFFF)
#define PT_VADDR(i) (MEMORY_LAYOUT_PAGING_STRUCTURES_START + PT_LENGTH*sizeof(pte_t)*i)

vmm_context_t kernel_context; /// the context for initalisation

/**
 * @fn ARCH_INIT_PAGING
 * Initalize paging (kernel_context)
 *
 * @param mb_info multiboot structure
 * @return void
 */
void ARCH_INIT_VMM(struct multiboot_struct *mb_info)
{
    paddr_t pframe = pmm_alloc_page_limit(0);
    vaddr_t vframe = MEMORY_LAYOUT_PAGING_STRUCTURES_START;

    kernel_context.arch_context.phys_addr = pframe;
    kernel_context.arch_context.entries = (pde_t *) pframe + MEMORY_LAYOUT_KERNEL_START;
    memset(kernel_context.arch_context.entries, 0, PAGE_SIZE);

    kernel_context.arch_context.entries[PDE_INDEX(vframe)] = pframe | VMM_WRITABLE | VMM_PRESENT;
    vmm_map_range(&kernel_context, 0, MEMORY_LAYOUT_KERNEL_START, NUM_PAGES(MEMORY_LAYOUT_DIRECT_MAPPED), VMM_WRITABLE);// kernel
    vmm_map(&kernel_context, 0xB8000, 0xC00B8000, VMM_WRITABLE);// videomemory (0xB8000 - 0xBFFFF)
    // multiboot
    vmm_map(&kernel_context, ((vaddr_t)mb_info & (~0xfff)) - MEMORY_LAYOUT_KERNEL_START, ((paddr_t)mb_info&(~0xfff)), VMM_WRITABLE);
    vmm_map(&kernel_context, (mb_info->mods_addr & (~0xfff)) - MEMORY_LAYOUT_KERNEL_START, mb_info->mods_addr & (~0xfff), VMM_WRITABLE);

    int i;
    uintptr_t addr;
    struct mods_add *modules = (void*) mb_info->mods_addr;
    for(i = 0; i < mb_info->mods_count; i++)
    {
        addr = modules[i].mod_start & (~0xfff);
        while(addr < modules[i].mod_end)
        {
            vmm_map(&kernel_context, addr, addr, VMM_PRESENT | VMM_WRITABLE);
            addr += PAGE_SIZE;
        }
    }

    void *pd_vaddr = (void *) vmm_automap_kernel(&kernel_context, pframe, VMM_WRITABLE);
    kernel_context.arch_context.entries = pd_vaddr;

    arch_vmm_switch_context(&kernel_context.arch_context);
    current_context = &kernel_context;
}

/**
 * @fn arch_map
 * @brief Map a physical address to a virtual adress
 *
 * @param context pagedirectory in that will map
 * @param pframe physical adress
 * @param vframe vitual adress
 * @param flags flags
 * @return success
 */
int arch_vmm_map(arch_vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags)
{
    if( (pframe & 0xFFF) || (vframe & 0xFFF) )
    {
        printf("pd_map(): can't map 0x%x to 0x%x.", pframe, vframe);
        return -1;
    }

    uint32_t pd_index = PDE_INDEX(vframe);
    uint32_t pt_index = PTE_INDEX(vframe);

    pt_t pt = NULL;
    pde_t pde = context->entries[pd_index];

    if (pde & VMM_PRESENT)
    {
        pt = arch_pt_get(context, pd_index);
    }
    else
    {
        pt = arch_pt_create(context, pd_index, VMM_PRESENT | flags);
    }

    pt[pt_index] = (pte_t)(pframe & ~0xFFF) | VMM_PRESENT | (flags & 0xFFF);

    if(current_context != NULL)
    {
        if(context == &current_context->arch_context)
        {
            arch_flush_tlb(vframe);
        }
        else
        {
            vmm_unmap(current_context, (vaddr_t) pt);
        }
    }

    return 0;
}

/**
 * Remove a mapping from pagedir/pagetable
 *
 * @param pd pagedirectory
 * @param frame adress to unmap
 *
 * @return void
 */
int arch_vmm_unmap(arch_vmm_context_t *context, vaddr_t frame)
{
    pt_t pt = arch_pt_get(context, PDE_INDEX(frame));
    pt[PTE_INDEX(frame)] = 0;

    int pt_emty = 1, i;
    for(i = 0; i < PT_LENGTH; i++)
    {
        if(pt[i])
        {
            pt_emty = 0;
            break;
        }
    }

    if(pt_emty)
    {
        arch_pt_destroy(context, PDE_INDEX(frame));
    }
    else
    {
        if(context != &current_context->arch_context)
            vmm_unmap(current_context, (vaddr_t) pt);
    }

    return 0;
}


/**
 * @fn arch_vaddr_find
 * @brief Find a free virtual adress
 *
 * @param context context
 * @param num number of pages to find
 * @param limit_low the lowest limit of the adress
 * @param limit_high the highest limit of the adress
 * @param flags paging flags for temporary mappings
 * @return virtual start adress
 */
vaddr_t arch_vaddr_find(arch_vmm_context_t *context, int num, vaddr_t limit_low, vaddr_t limit_high)
{
#define PAGES_FOUND(l) \
	  if(vaddr == (vaddr_t)NULL) { \
	    page = pd_index * PT_LENGTH + pt_index; \
	    vaddr = page * PAGE_SIZE; \
	  } \
	  pages_found += l; \
	  if(pages_found >= num) { \
	    return vaddr; \
	  }

    vaddr_t vaddr = 0;
    int page = 0;
    int pages_found = 0;

    uint32_t pd_index = PDE_INDEX(limit_low);
    uint32_t pt_index = PTE_INDEX(limit_low);
    uint32_t pd_index_end = PDE_INDEX(limit_high);
    uint32_t pt_index_end = PTE_INDEX(limit_high);
    pt_t pt;
    while(pd_index <= pd_index_end)
    {
        if(context->entries[pd_index] & VMM_PRESENT)
        {
            pt = arch_pt_get(context, pd_index);

            uint32_t pt_end = (pd_index == pd_index_end) ? pt_index_end : PT_LENGTH; // last pd entry
            for(; pt_index < pt_end; pt_index++)
            {
                if(! ((uint32_t)pt[pt_index] & VMM_PRESENT) )
                {
                    PAGES_FOUND(1);
                }
                else
                {
                    pages_found = 0;
                    vaddr = (uintptr_t)NULL;
                }
            }

            pt_index = 0;
            if(context != &current_context->arch_context)
                vmm_unmap(current_context, (vaddr_t)pt);
        }
        else
        {
            PAGES_FOUND(PT_LENGTH);
        }
        pd_index++;
    }

    return 0;
}

/**
 * @fn arch_vmm_is_present
 * @brief check if a page is present
 *
 * @param context context
 * @param vaddr adress to check
 * @return true, if is present; false, if not
 */
int arch_vmm_is_present(arch_vmm_context_t *context, vaddr_t vaddr)
{
    unsigned int pd_index = PDE_INDEX(vaddr);
    unsigned int pt_index = PTE_INDEX(vaddr);

    if(context->entries[pd_index] & VMM_PRESENT)
    {
        pt_t *pt = (pt_t *)arch_pt_get(context, pd_index);
        return ((uint32_t)pt[pt_index] & VMM_PRESENT) ? 1 : 0;
    }

    return 0;
}

/**
 * @fn arch_vaddr2paddr
 * @brief get the physical adress from a mapped virtual adress.
 *
 * @param context context
 * @param vaddr the virtual adress
 * @return the physical adress
 */
paddr_t arch_vaddr2paddr(arch_vmm_context_t *context, vaddr_t vaddr)
{
    unsigned int pd_index = PDE_INDEX(vaddr);
    unsigned int pt_index = PTE_INDEX(vaddr);

    if(context->entries[pd_index] & VMM_PRESENT)
    {
        pt_t *pt = (pt_t *) arch_pt_get(context, pd_index);
        return (paddr_t) pt[pt_index] & ~0xfff;
    }
    return 0;
}
/**
 * @fn page_fault_handler
 * @brief Exception handler for pagefaults
 *
 * @param cpu current cpu state
 * @return void
 */
void page_fault_handler(struct cpu_state **cpu_p)
{
    struct cpu_state *cpu = cpu_p[0];
    char message[512];

    uint32_t addr;
    asm volatile("mov %%cr2, %0" : "=r" (addr));

    sprintf(message, "Page fault in %s space:\nError %s address %#010X: %s.\nEIP: %#010X\n Process: %s\n", ((cpu->error & 4) ? "user" : "kernel"),
            ((cpu->error & 2) ? "writing to" : "reading at"), addr, ((cpu->error & 1) ? "Access denied" : "Nonpaged area"), cpu->eip,current_thread->process->name);

    panic(message);
}

/**
 * @fn arch_flush_tlb
 */
static inline void arch_flush_tlb(vaddr_t addr)
{
    asm volatile ("invlpg %0" : : "m" (*(char*) addr));
}

