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

#include <pmm.h>
#include <math.h>
#include <multiboot.h>
#include <printf.h>
#include <panic.h>


/*
 * NOTE: Most of these functions are not threadsafe!
 */

/* Memory for DMA */
#define PMM_DMA_LIMIT 16 * 1024 * 1024 // 16 MB

unsigned long pmm_mmap[PMM_MMAP_SIZE];

/**
 * Returns the number of free pages
 *
 * @return number of free pages
 */
unsigned long pmm_count_free_pages() {
	unsigned long free_pages = 0;
	unsigned long i, z;

	for (i = 0; i < PMM_MMAP_SIZE; i++) {
		for (z = 0; z < 32; z++) {
			if (pmm_mmap[i] & (1 << z)) {
				free_pages++;
			}
		}
	}
	return free_pages;
}

/**
 * Mark page als free
 *
 * @param page-pointer on the begin of the page which should be marked as free.
 */
void pmm_mark_page_as_free(paddr_t page)
{ //TODO: should that be possible from the entire kernel?
	pmm_mmap[page / PAGE_SIZE / 32] |= 1 << ((page / PAGE_SIZE) & 31);
}

/**
 * Mark num pages as free
 *
 * @praram page-pointer on the begin of the first page.
 * @param num of the pages which should be marked as free.
 */
static void pmm_mark_page_range_as_free(paddr_t page, unsigned int num)
{
	int i;

	if (!num)
		panic("PMM: pmm_mark_page_range_as_free(): num is zero");

	if (page / PAGE_SIZE + num > PMM_MMAP_SIZE * 32)
			panic("PMM: pmm_mark_page_range_as_free(): marking the given\n"
		"pages as free would cause a buffer overrun");


	for (i = 0; i < num; i++) {
		pmm_mark_page_as_free(page + i * PAGE_SIZE);
	}
}

/**
 * mark page as used
 *
 * @praram page-pointer on the begin of the first page.
 */
static void pmm_mark_page_as_used(paddr_t page)
{
	pmm_mmap[page / PAGE_SIZE / 32] &= ~(1 << ((page / PAGE_SIZE) & 31));
}

/**
 * Mark num pages as used
 *
 * @praram page-pointer on the begin of the first page.
 * @param num of the pages which should be marked as free.
 */
static void pmm_mark_page_range_as_used(paddr_t page, unsigned int num)
{
	int i;

	if (!num)
		panic("PMM: pmm_mark_page_range_as_used(): num is zero");

/*
 * PMM is broken, see comment in PMM_INIT
 */
/*	if (page / PAGE_SIZE + num > PMM_MMAP_SIZE * 32)
		panic("PMM: pmm_mark_page_range_as_used(): marking the given\n"
		    "pages as used would cause a buffer overrun");
*/
	for (i = 0; i < num; i++) {
		pmm_mark_page_as_used(page + i * PAGE_SIZE);
	}
}

/**
 * Search for a free page and returns a pointer on the begin.
 *
 * @param lower_limit minimus size of the page.
 *
 * @return If the search is succesful this method returns a pointer on the begin on this page.
 */
static paddr_t pmm_find_free_page(unsigned long lower_limit)
{
	uint32_t i, z;
	paddr_t page = 0;

	i = lower_limit / PAGE_SIZE / 32;
	if (pmm_mmap[i] & (0xffffffff << (( lower_limit / PAGE_SIZE) % 32))) {
		z = bit_scan_forward(pmm_mmap[i] & (0xffffffff << ((lower_limit / PAGE_SIZE) % 32)));
		page = (i * 32 + z) * PAGE_SIZE;
		return page;
	}

	for (i++; i < PMM_MMAP_SIZE; i++) {
		if (pmm_mmap[i]) {
			z = bit_scan_forward(pmm_mmap[i]);
			page = (i * 32 + z) * PAGE_SIZE;
			return page;
		}
	}

	/* checked in the alloc functions */
	return -1;
}

/**
 * Searchs for num free pages and returns pointer on the begin of the first page.
 *
 * @param lower_limit minimum address of the first page.
 * @param num Number of pages.
 *
 * @return Pointer on begin of the first page.
 * @return If succesful this method will return a pointer on the begin of the first free page.
 */
static unsigned int pmm_find_free_page_range(unsigned long lower_limit, unsigned int num)
{
	uint32_t i, z;
	uint32_t found = 0;
	paddr_t page = 0;

	if (!num)
		panic("PMM: searching 0 pages (find_free_page_range())");

	for (i = lower_limit / PAGE_SIZE / 32; i < PMM_MMAP_SIZE; i++) {
		printf("%i\n", i);
		if (pmm_mmap[i] == 0) {
			found = 0;
			continue;
		}

		if (pmm_mmap[i] == 0xffffffff) {
			if (found = 0) {
				page = (i * 32) * PAGE_SIZE; //
			}
			found += 32;

		} else {
			printf("bar\n");
			for (z = 0; z < 32; z++) {
				if (pmm_mmap[i] & (1 << z)) {
					if (found == 0) {
						page = (i * 32 + z) * PAGE_SIZE;
					}
					found++;
					if (found >= num) {
						return page;
					}
				} else {
					found = 0;
				}
			}
		}

		if (found >= num) {
			return page;
		}
	}

	return -1; /* checked in the alloc functions */
}

/**
 * Reserve a page and mark this page as used.
 *
 * @return Pointer on the begin of the page.
 */
paddr_t pmm_alloc_page()
{
	paddr_t page = pmm_find_free_page(PMM_DMA_LIMIT);

	if (page & (PAGE_SIZE - 1)) {
		panic("PMM: pmm_alloc_page(): no pages left");
	}
	pmm_mark_page_as_used(page);
	return page;
}

/**
 * Reserve a page not under a entered address
 * The minimum is PMA_DMA_LIMIT.
 */
paddr_t pmm_alloc_page_limit(uint32_t lower_limit)
{
	paddr_t page;
	if (PMM_DMA_LIMIT > lower_limit)
		page = pmm_find_free_page(PMM_DMA_LIMIT);
	else
		page = pmm_find_free_page(lower_limit);

	if (page & (PAGE_SIZE - 1)) {
		panic("PMM: pmm_alloc_page_limit(): no pages left");
	}

	pmm_mark_page_as_used(page);
	return page;
}


/**
 * Reserve num DMA-Pages.
 *
 * @return Pointer on the begin of the first page.
 */
paddr_t pmm_alloc_dma_page_range(unsigned int num)
{
	if (!num)
		panic("PMM: pmm_alloc_dma_page_range(): num zero");
	paddr_t page = pmm_find_free_page_range(0, num);
	if (page & (PAGE_SIZE - 1) || page >= PMM_DMA_LIMIT) {
		panic("PMM: pmm_alloc_dma_page_range(): no DMA memory left");
	}
	pmm_mark_page_range_as_used(page, num);
	return page;
}

/**
 * Reserve num pages.
 *
 * @return Pointer on the begin of the first page.
 */
paddr_t pmm_alloc_page_range(unsigned int num)
{
	if (!num)
		panic("PMM: pmm_alloc_page_range(): num zero");
	paddr_t page = pmm_find_free_page_range(PMM_DMA_LIMIT, num);
	if (page & (PAGE_SIZE - 1)) {
		panic("PMM: pmm_alloc_page_range(): no memory left");
	}
	pmm_mark_page_range_as_used(page, num);
	return page;
}

void INIT_PMM(struct multiboot_struct* MBS)
{
	unsigned long i;
	int x;
	struct mmap_entry *GRUB_MMAP = (struct mmap_entry *)MBS->mmap_addr;
	uint32_t memsize; /* in bytes */

	for (i = 0; i < PMM_MMAP_SIZE; i++)
		pmm_mmap[i]=0xFFFFFFFF;

	while (i <= (MBS->mmap_length / 24)) {
		if (GRUB_MMAP[i].Type == 1) {
			uint32_t base = (uint32_t)GRUB_MMAP[i].BaseAddr;
			uint32_t limit = (((uint32_t)GRUB_MMAP[i].Length) + base);
			while (base < limit) {
				pmm_mark_page_as_used((paddr_t)base);
				base += 0x1000;
			}
		}
		i++;
	}

	//protect Memory structures
	pmm_mark_page_as_used(0); //IVT+BDA

	uint16_t* EBDA_p = (void *)0x040E;
	pmm_mark_page_as_used((paddr_t)EBDA_p[0] << 4); //EBDA

	uint16_t* BDA_size=0x0413;
	pmm_mark_page_as_used((BDA_size[0] / 4) * 1024); //FPS (maybe)

	printf("ROM-AREA\n");
	pmm_mark_page_range_as_used(0xA0000, 96); //0xA0000 - 0xFFFFF ROM-AREA

	//multiboot structures
	struct mods_add* MBMA = (void*)MBS->mods_addr;
	pmm_mark_page_as_used((paddr_t)MBS);
	pmm_mark_page_as_used((paddr_t)MBMA);

	//multiboot modules
	if (MBS->mods_count > 0) {
		for (x = 0; x < MBS->mods_count; x++) {
			uint32_t size = MBMA[x].mod_end-MBMA[x].mod_start;
			if (size % 4096 != 0) {
				size /= 4096;
				size++;
			} else {
				size /= 4096;
			}
			pmm_mark_page_range_as_used((paddr_t) MBMA[x].mod_start, size);
			pmm_mark_page_as_used((paddr_t) MBMA[x]. string);
		}
	}

/*
 * FIXME TODO XXX
 * kernel_start and kernel_end are currently broken, AND SO IS PMM
 * kernel_end is lower than kernel_start
 */
	// Kernel
	pmm_mark_page_range_as_used((paddr_t)&kernel_start, ((uint32_t)&kernel_end - (uint32_t)&kernel_start) / PAGE_SIZE);

	// end of phys. memory
	if (MBS->flags & 0x1) {		/* check for high- and lowmem field */
		printf("RAM: %uMB\n", (MBS->mem_low + MBS->mem_up) / 1024);
		memsize = (MBS->mem_low + MBS->mem_up) * 1024;
/*
 * FIXME TODO XXX
 * something with the paging is broken
 */
//		pmm_mark_page_range_as_used(memsize, (PMM_MMAP_SIZE * 32 - (memsize / PAGE_SIZE)));
	} else {
		panic("PMM_INIT: no ram info in multiboot structure");
	}
}

