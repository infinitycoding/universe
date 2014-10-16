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
 *  @file /arch/x86/pmm.c
 *  @brief Kernel module for managin physical memory
 *  @todo Replace static memory map with a dynamic list.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <pmm.h>
#include <math.h>
#include <sys/multiboot.h>
#include <mm/paging.h>
#include <memory_layout.h>
#include <printf.h>
#include <panic.h>


/*
 * NOTE: Most of these functions are not threadsafe!
 */

/* Memory for DMA */
#define PMM_DMA_LIMIT 16 * 1024 * 1024 // 16 MB

unsigned long pmm_mmap[PMM_MMAP_SIZE];


/**
 * @brief Returns the number of free pages.
 * @return number of free pages
 */
int pmm_count_free_pages(void)
{
    int free_pages = 0;
    int i, z;

    for (i = 0; i < PMM_MMAP_SIZE; i++)
    {
        for (z = 0; z < 32; z++)
        {
            if (pmm_mmap[i] & (1 << z))
            {
                free_pages++;
            }
        }
    }
    return free_pages;
}


/**
 * @brief Checs if the page is already in use or not.
 * @param page the pysical base adress of the page
 * @return 0 = page is already taken; 1 = Page is free
 */
int pmm_check_page(paddr_t page)
{
    return pmm_mmap[page / PAGE_SIZE / 32] & (1 << ((page / PAGE_SIZE) & 31));
}

/**
 * @brief Checs if all pages from page to page+PAGE_SIZE*(range-1) are in use or not.
 * @param page the pysical base adress of the page
 * @param range number of pages to be checked
 * @return 0 = page is already taken; 1 = Page is free
 */
int pmm_check_page_range(paddr_t page, size_t range)
{
    int r = 1;
    size_t i;
    for(i=0; i < range; i++)
    {
        r &= pmm_check_page(page);
        page += PAGE_SIZE;
    }
    return r;
}


/**
 * @brief Mark page als free.
 * @param page pointer on the begin of the page which should be marked as free.
 */
void pmm_mark_page_as_free(paddr_t page)
{
    //TODO: should that be possible from the entire kernel?
    pmm_mmap[page / PAGE_SIZE / 32] |= 1 << ((page / PAGE_SIZE) & 31);
}


/**
 * @brief Mark num pages as free.
 * @param page pointer on the begin of the first page.
 * @param num number of the pages which should be marked as free.
 */
void pmm_mark_page_range_as_free(paddr_t page, unsigned int num)
{
    int i;

    if (!num)
        panic("PMM: pmm_mark_page_range_as_free(): num is zero");

    if (page / PAGE_SIZE + num > PMM_MMAP_SIZE * 32)
        panic("PMM: pmm_mark_page_range_as_free(): marking the given\n"
              "pages as free would cause a buffer overrun");


    for (i = 0; i < num; i++)
    {
        pmm_mark_page_as_free(page + i * PAGE_SIZE);
    }
}


/**
 * @brief Mark page as used.
 * @param page pointer on the begin of the first page.
 */
void pmm_mark_page_as_used(paddr_t page)
{
    pmm_mmap[page / PAGE_SIZE / 32] &= ~(1 << ((page / PAGE_SIZE) & 31));
}


/**
 * @brief Mark num pages as used.
 * @param page pointer on the begin of the first page.
 * @param num number of the pages which should be marked as free.
 */
void pmm_mark_page_range_as_used(paddr_t page, unsigned int num)
{
    int i;

    if (!num)
        panic("PMM: pmm_mark_page_range_as_used(): num is zero");

    /*
     * PMM is broken, see comment in PMM_INIT
     */
    if (page / PAGE_SIZE + num > PMM_MMAP_SIZE * 32)
        panic("PMM: pmm_mark_page_range_as_used(): marking the given\n"
              "pages as used would cause a buffer overrun");

    for (i = 0; i < num; i++)
    {
        pmm_mark_page_as_used(page + i * PAGE_SIZE);
    }
}


/**
 * @brief Search for a free page and returns a pointer on the begin.
 * @param lower_limit minimus size of the page.
 * @return If the search is succesful this method returns a pointer on the begin on this page.
 */
paddr_t pmm_find_free_page(unsigned long lower_limit)
{
    uint32_t i, z;
    paddr_t page = 0;

    i = lower_limit / PAGE_SIZE / 32;
    if (pmm_mmap[i] & (0xffffffff << (( lower_limit / PAGE_SIZE) % 32)))
    {
        z = bit_scan_forward(pmm_mmap[i] & (0xffffffff << ((lower_limit / PAGE_SIZE) % 32)));
        page = (i * 32 + z) * PAGE_SIZE;
        return page;
    }

    for (i++; i < PMM_MMAP_SIZE; i++)
    {
        if (pmm_mmap[i])
        {
            z = bit_scan_forward(pmm_mmap[i]);
            page = (i * 32 + z) * PAGE_SIZE;
            return page;
        }
    }

    /* checked in the alloc functions */
    return -1;
}


/**
 * @brief Searchs for num free pages and returns pointer on the begin of the first page.
 * @param lower_limit minimum address of the first page.
 * @param num Number of pages.
 * @return Pointer on begin of the first page.
 * If succesful this method will return a pointer on the begin of the first free page.
 */
paddr_t pmm_find_free_page_range(unsigned long lower_limit, unsigned int num)
{
    uint32_t i, z;
    uint32_t found = 0;
    paddr_t page = 0;

    if (!num)
        panic("PMM: searching 0 pages (find_free_page_range())");

    for (i = lower_limit / PAGE_SIZE / 32; i < PMM_MMAP_SIZE; i++)
    {
        if (pmm_mmap[i] == 0)
        {
            found = 0;
            continue;
        }

        if (pmm_mmap[i] == 0xffffffff)
        {
            if (found == 0)
            {
                page = (i * 32) * PAGE_SIZE; //
            }
            found += 32;

        }
        else
        {
            for (z = 0; z < 32; z++)
            {
                if (pmm_mmap[i] & (1 << z))
                {
                    if (found == 0)
                    {
                        page = (i * 32 + z) * PAGE_SIZE;
                    }
                    found++;

                    if (found >= num)
                    {
                        return page;
                    }
                }
                else
                {
                    found = 0;
                }
            }
        }

        if (found >= num)
        {
            return page;
        }
    }

    return -1; /* checked in the alloc functions */
}


/**
 * @brief Reserve a page and mark this page as used.
 * @return Pointer on the begin of the page.
 */
paddr_t pmm_alloc_page(void)
{
    paddr_t page = pmm_find_free_page(PMM_DMA_LIMIT);

    if (page & (PAGE_SIZE - 1))
    {
        panic("PMM: pmm_alloc_page(): no pages left");
    }
    pmm_mark_page_as_used(page);

    return page;
}


/**
 * @brief Reserve a page not under a entered address
 * @param lower_limit the lower limit
 * @return Pointer on the begin of the allocated page.
 */
paddr_t pmm_alloc_page_limit(paddr_t lower_limit)
{
    paddr_t page = pmm_find_free_page(lower_limit);

    if (page & (PAGE_SIZE - 1))
    {
        panic("PMM: pmm_alloc_page_limit(): no pages left");
    }

    pmm_mark_page_as_used(page);
    return page;
}


/**
 * @brief Reserve num DMA-Pages.
 * @param num the number of pages to be allocated
 * @return Pointer on the begin of the first page.
 */
paddr_t pmm_alloc_dma_page_range(unsigned int num)
{
    if (!num)
        panic("PMM: pmm_alloc_dma_page_range(): num zero");
    paddr_t page = pmm_find_free_page_range(0, num);
    if (page & (PAGE_SIZE - 1) || page >= PMM_DMA_LIMIT)
    {
        panic("PMM: pmm_alloc_dma_page_range(): no DMA memory left");
    }
    pmm_mark_page_range_as_used(page, num);
    return page;
}


/**
 * @brief Reserve num pages.
 * @return Pointer on the begin of the first page.
 */
paddr_t pmm_alloc_page_range(unsigned int num)
{
    if (!num)
        panic("PMM: pmm_alloc_page_range(): num zero");
    paddr_t page = pmm_find_free_page_range(PMM_DMA_LIMIT, num);
    if (page & (PAGE_SIZE - 1))
    {
        panic("PMM: pmm_alloc_page_range(): no memory left");
    }
    pmm_mark_page_range_as_used(page, num);
    return page;
}


/**
 * @brief Initiates the PMM module.
 * @param mb_info the multiboot info struct for module protection
 */

void INIT_PMM(struct multiboot_struct *mb_info)
{
    int i;
    for (i = 0; i < PMM_MMAP_SIZE; i++)
    {
        pmm_mmap[i] = 0;
    }

    mb_info->mmap_addr += MEMORY_LAYOUT_KERNEL_START;
    mb_info->mods_addr += MEMORY_LAYOUT_KERNEL_START;

    struct mmap_entry *mmap = (struct mmap_entry *)mb_info->mmap_addr;

    int len = 0;
    for(i = 0; len < mb_info->mmap_length; i++)
    {
        len += mmap[i].size +4;
        if(mmap[i].Type == 1)
        {
            uintptr_t addr = mmap[i].BaseAddr;
            int pages = mmap[i].Length / PAGE_SIZE;

            pmm_mark_page_range_as_free(addr, pages);
        }
    }

    //protect Memory structures
    pmm_mark_page_as_used(0); //IVT+BDA

    uint16_t* EBDA_p = (void *)0x040E;
    pmm_mark_page_as_used((paddr_t)EBDA_p[0] << 4); //EBDA

    uint16_t* BDA_size = (uint16_t*)0x0413;
    pmm_mark_page_as_used((BDA_size[0] / 4) * 1024); //FPS (maybe)
    pmm_mark_page_range_as_used(0xA0000, 96); //0xA0000 - 0xFFFFF ROM-AREA


    //multiboot structures
    struct mods_add *mods = (void*)mb_info->mods_addr;
    pmm_mark_page_as_used((paddr_t)mb_info - MEMORY_LAYOUT_KERNEL_START);
    pmm_mark_page_as_used((paddr_t)mods    - MEMORY_LAYOUT_KERNEL_START);

    //multiboot modules
    for (i = 0; i < mb_info->mods_count; i++)
    {
        uint32_t size = mods[i].mod_end - mods[i].mod_start;
        int pages = NUM_PAGES(size);
        pmm_mark_page_range_as_used((paddr_t) mods[i].mod_start, pages);
        pmm_mark_page_as_used((paddr_t) mods[i].string);
    }

    if (! (mb_info->flags & 0x1))
    {
        panic("PMM_INIT: no ram info in multiboot structure");
    }

}

