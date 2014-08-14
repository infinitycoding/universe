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
 *  @file /mm/paging.c
 *  @brief common paging wrappers and additional functions
 *
 *  @author Michael Sippel <micha@infinitycoding.de>
 */

#include <mm/paging.h>
#include <arch_paging.h>
#include <memory_layout.h>
#include <idt.h>
#include <sched/thread.h>

vmm_context_t *current_context = NULL; /// @var current_context pointer to the current memory context

/**
 * @fn INIT_PAGING
 * @brief Initalize the virtual memory management.
 * Setup the hardware for paging and create basic mappings.
 *
 * @param mb_info pointer to multiboot structure
 * @return void
 */
void INIT_PAGING(struct multiboot_struct *mb_info)
{
    install_exc(0xe, page_fault_handler);
    ARCH_INIT_PAGING(mb_info);
}

/**
 * @fn vmm_create_context
 * @brief Initalizes a new vmm context from a pointer.
 * The memory must be allocated before.
 *
 * @param context pointer to the memory structure
 * @return void
 */
void vmm_create_context(vmm_context_t *context)
{
    arch_vmm_create_context(&context->arch_context);
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
    arch_vmm_destroy_context(&context->arch_context);
}

/**
 * @fn vmm_switch_context
 * @brief Change the vmm context running currently on hardware
 *
 * @param context pointer to context
 * @return void
 */
void vmm_switch_context(vmm_context_t *context)
{
    if(context != current_context)
    {
        arch_update_context(&context->arch_context);
        arch_switch_context(&context->arch_context);
        current_context = context;
    }
}

/**
 * @fn vmm_map
 * @brief Map a physical adress to a virtual adress.
 * The adresses must be 4k-aligned as in all other following mapping functions too.
 *
 * @param context the context
 * @param pframe the physical adress
 * @param vframe virtual adress
 * @param flags additional paging flags (defined /include/arch/x86/arch_paging.h)
 * @return success
 */
int vmm_map(vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags)
{
    return arch_map(&context->arch_context, pframe, vframe, flags);
}

/**
 * @fn vmm_unmap
 * @brief free a virtual adress from the mapping
 *
 * @param context the vmm context
 * @param frame the adress to free
 * @return success
 */
int vmm_unmap(vmm_context_t *context, vaddr_t frame)
{
    return arch_unmap(&context->arch_context, frame);
}

/**
 * @fn vmm_map_range
 * @brief map a rage of pages
 *
 * @param context the context
 * @param pframe physical start adress
 * @param vframe virtual start adress
 * @param pages number of pages / size of the range to map
 * @param flags paging flags (same for vmm_map)
 * @return success
 */
int vmm_map_range(vmm_context_t *context, paddr_t pframe, vaddr_t vframe, int pages, uint8_t flags)
{
    int p;
    for (p = 0; p < pages; ++p)
    {
        vmm_map(context, pframe + PAGE_FRAME_ADDR(p), vframe + PAGE_FRAME_ADDR(p), flags);
    }
    return 0;
}

/**
 * @fn vmm_unmap_range
 * @brief free a range in virtual memory from mappings
 *
 * @param context context
 * @param frame the start adress in virtual space
 * @param pages number of pages / length of the range
 * @return success
 */
int vmm_unmap_range(vmm_context_t *context, vaddr_t frame, int pages)
{
    int p;
    for(p = 0; p < pages; p++)
    {
        vmm_unmap(context, frame + PAGE_FRAME_ADDR(p));
    }
    return 0;
}

/**
 * @fn vmm_automap_kernel
 * @brief map a physical adress to an automatically searched free virtual adress in kernelspace.
 * Searches a free virtual adress in kernel-adress-space with arch_vaddr_find,
 * maps it to the given physical adress and returns the found virtual adress.
 *
 * @param context context
 * @param pframe the physical adress of the page
 * @param flags paging flags (same for vmm_map)
 * @return the virtual adress
 */
vaddr_t vmm_automap_kernel(vmm_context_t *context, paddr_t pframe, uint8_t flags)
{
    vaddr_t vframe = arch_vaddr_find(&context->arch_context, 1,
                                     MEMORY_LAYOUT_RESERVED_AREA_END,
                                     MEMORY_LAYOUT_KERNEL_HEAP_START, flags);
    vmm_map(context, pframe, vframe, flags | VMM_PRESENT);

    return vframe;
}

/**
 * @fn vmm_automap_kernel_range
 * @brief map automatically a range in kernelspace.
 * Searches a free virtual adress range in kernel-adress-space with arch_vaddr_find,
 * maps it to the given physical adress range and returns the found virtual start adress.
 *
 * @param context context
 * @param pframe physical start adress
 * @param pages number of pages to map
 * @param flags paging flags (same as vmm_map)
 * @return the virtual start adress
 */
vaddr_t vmm_automap_kernel_range(vmm_context_t *context, paddr_t pframe, int pages, uint8_t flags)
{
    int i;
    vaddr_t vaddr_start = arch_vaddr_find(&context->arch_context, pages, MEMORY_LAYOUT_RESERVED_AREA_END, MEMORY_LAYOUT_KERNEL_HEAP_START, flags);
    for(i = 0; i < pages; i++)
    {
        paddr_t paddr = pframe + i*PAGE_SIZE;
        vaddr_t vaddr = vaddr_start + i*PAGE_SIZE;
        vmm_map(context, paddr, vaddr, flags);
    }

    return vaddr_start;
}

/**
 * @fn vmm_automap_user
 * @brief map a physical adress to an automatically searched free virtual adress in userspace.
 * Searches a free virtual adress in user-adress-space with arch_vaddr_find,
 * maps it to the given physical adress and returns the found virtual adress.
 *
 * @param context context
 * @param pframe the physical adress of the page
 * @param flags paging flags (same for vmm_map)
 * @return the virtual adress
 */
vaddr_t vmm_automap_user(vmm_context_t *context, paddr_t pframe, uint8_t flags)
{
    vaddr_t vframe = arch_vaddr_find(&context->arch_context, 1,
                                     0x0, MEMORY_LAYOUT_KERNEL_START, flags);
    vmm_map(context, pframe, vframe, flags | VMM_PRESENT);

    return vframe;
}

/**
 * @fn vmm_automap_user_range
 * @brief map automatically a range in userspace.
 * Searches a free virtual adress range in user-adress-space with arch_vaddr_find,
 * maps it to the given physical adress range and returns the found virtual start adress.
 *
 * @param context context
 * @param pframe physical start adress
 * @param pages number of pages to map
 * @param flags paging flags (same as vmm_map)
 * @return the virtual start adress
 */
vaddr_t vmm_automap_user_range(vmm_context_t *context, paddr_t pframe, int pages, uint8_t flags)
{
    int i;
    vaddr_t vaddr_start = arch_vaddr_find(&context->arch_context, pages, 0x0, MEMORY_LAYOUT_KERNEL_START, flags);
    for(i = 0; i < pages; i++)
    {
        paddr_t paddr = pframe + i*PAGE_SIZE;
        vaddr_t vaddr = vaddr_start + i*PAGE_SIZE;
        vmm_map(context, paddr, vaddr, flags);
    }

    return vaddr_start;
}

void alloc_memory(struct cpu_state **cpu)
{
    int pages = (*cpu)->CPU_ARG1;

    uint32_t *dest = (uint32_t *)arch_vaddr_find(&current_context->arch_context, pages, MEMORY_LAYOUT_USER_HEAP_START, MEMORY_LAYOUT_USER_HEAP_END, VMM_PRESENT|VMM_WRITABLE|VMM_USER);

    int i;
    for(i = 0; i < pages; i++)
    {
        uint32_t *paddr = (uint32_t *)pmm_alloc_page();
        uint32_t *vaddr = dest + i*PAGE_SIZE;
        vmm_map(current_context, (paddr_t)paddr, (vaddr_t)vaddr, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
    }
    (*cpu)->CPU_ARG0 = (uint32_t)dest;
}


extern struct thread_state* current_thread; /// defined in /sched/scheduler.c

void sys_brk(struct cpu_state **cpu)
{
    uint32_t new_bss_top = (*cpu)->CPU_ARG1;
    uint32_t heap_top = current_thread->process->heap_top;
    if(new_bss_top == 0)
    {
        (*cpu)->CPU_ARG0 = heap_top;
    }
    else if(new_bss_top > current_thread->process->heap_upper_limit || new_bss_top < current_thread->process->heap_lower_limit)
    {
        (*cpu)->CPU_ARG0 = 0;
    }
    else if(new_bss_top > (heap_top|0x00000FFF))
    {
        int req_memory = new_bss_top-((heap_top|0x00000FFF));
        int pages = 1+(req_memory/PAGE_SIZE);
        if(req_memory%PAGE_SIZE)
            pages++;

        heap_top = arch_vaddr_find(&current_context->arch_context, pages, MEMORY_LAYOUT_USER_HEAP_START, MEMORY_LAYOUT_USER_HEAP_END, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        int i;
        for(i = 0; i < pages; i++)
        {
            paddr_t paddr = pmm_alloc_page();
            vaddr_t vaddr = heap_top + i*PAGE_SIZE;
            vmm_map(current_context, paddr, vaddr, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        }

        heap_top += req_memory;
    }
    else if(new_bss_top < (heap_top^0xFFFFF000))
    {
        //todo Free
    }
    else
        heap_top = new_bss_top;


    current_thread->process->heap_top = heap_top;
    (*cpu)->CPU_ARG0 = heap_top;
}
