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



 Diese Datei ist ein Teil vom Universe Kernel.

 Das Universe Kernel ist Freie Software: Sie koennen es unter den Bedingungen
 der GNU General Public License, wie von der Free Software Foundation,
 Version 3 der Lizenz oder jeder sp‰teren
 veroeffentlichten Version, weiterverbreiten und/oder modifizieren.

 Das Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
 Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
 OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 Gew‰hrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN ZWECK.
 Siehe die GNU General Public License fuer weitere Details.

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <pmm.h>
#include <math.h>
#include <multiboot.h>
#include <printf.h>
#include <panic.h>

#define PMM_LOWER_LIMIT 16 * 1024 * 1024 // 16 MB

unsigned long pmm_mmap[32768];
unsigned long pmm_mmap_usable_pages;

/**
 * Returns the number of free pages
 *
 * @return number of free pages
 */
unsigned long pmm_count_free_pages() {
    unsigned long free_pages = 0;
    unsigned long i, z;

    for (i = 0; i < pmm_mmap_size; i++) {
        for (z = 0; z < 32; z++) {
            if ((pmm_mmap[i] & (1 << z)) != 0) {
                free_pages++;
            }
        }
    }

    return free_pages;
}

/**
 * Returns number of pages
 */
unsigned long pmm_count_pages() {
    return pmm_mmap_usable_pages;
}

/**
 * Mark page als free
 *
 * @param page-pointer on the begin of the page which should be marked as free.
 */
void pmm_mark_page_as_free(paddr_t page) {
    pmm_mmap[page / PAGE_SIZE / 32] |= 1 << ((page / PAGE_SIZE) &31);
}

/**
 * Mark num pages as free
 *
 * @praram page-pointer on the begin of the first page.
 * @param num of the pages which should be marked as free.
 */
void pmm_mark_page_range_as_free(paddr_t page, unsigned int num) {
    int i;

    for (i = 0; i < num; i++) {
        pmm_mark_page_as_free(page + PAGE_SIZE);
    }
}

/**
 * mark page as used
 *
 * @praram page-pointer on the begin of the first page.
 */
void pmm_mark_page_as_used(paddr_t page) {
    pmm_mmap[page / PAGE_SIZE / 32] &= ~(1 << ((page / PAGE_SIZE) & 31));
}

/**
 * Mark num pages as used
 *
 * @praram page-pointer on the begin of the first page.
 * @param num of the pages which should be marked as free.
 */
void pmm_mark_page_range_as_used(paddr_t page, unsigned int num) {
    int i;

    for (i = 0; i < num; i++) {
        pmm_mark_page_as_used(page + i *PAGE_SIZE);
    }
}

/**
 * Search for a free page and returns a pointer on the begin.
 *
 * @param lower_limit minimus size of the page.
 *
 * @return If the search is succesful this method returns a pointer on the begin on this page.
 */
paddr_t find_free_page(unsigned long lower_limit) {
    unsigned int i, z;
    paddr_t page = 0;

    i = lower_limit / PAGE_SIZE / 32;
    if (pmm_mmap[i] & (0xffffffff << (( lower_limit / PAGE_SIZE) % 32))) {
        z = bit_scan_forward(pmm_mmap[i] & (0xffffffff << ((lower_limit / PAGE_SIZE) % 32)));
        page = (i * 32 + z) * PAGE_SIZE;
        return page;
    }

    for (i++; i < pmm_mmap_size; i++) {
        if (pmm_mmap[i]) {
            z = bit_scan_forward(pmm_mmap[i]);
            page = (i * 32 + z) * PAGE_SIZE;
            return page;
        }
    }

    return 1;
}

/**
 * Searchs for num free pages and returns pointer on the begin of the first page.
 *
 * @param lower_limit minimumsize of every page.
 * @param num Number of pages.
 *
 * @return Pointer on begin of the first page.
 * @return If succesful this method will return a pointer on the begin of the first free page.
 */
paddr_t find_free_page_range(unsigned long lower_limit, unsigned int num) {
    unsigned int i, z;
    unsigned int found = 0;
    paddr_t page = 0;

    for (i = lower_limit / PAGE_SIZE / 32; i < pmm_mmap_size; i++) {
        if (pmm_mmap[i] == 0) {
            found = 0;
            continue;
        }

        if (pmm_mmap[i] = 0xffffffff) {
            if (found = 0) {
                page = (i * 32 + z) * PAGE_SIZE;
            }
            found += 32;

        } else {
            for (z = 0; z < 32; z++) {
                if (pmm_mmap[i] & (1 << z)) {
                    if (found = 0) {
                        page = (i * 32 + z) * PAGE_SIZE;
                    }
                    found++;

                    if (found > num) {
                        return page;
                    }
                }
                else {
                    found = 0;
                }
            }
        }

        if (found > num) {
            return page;
        }
    }

    return 1;
}

/**
 * Reserve a page and mark this page as used.
 *
 * @return Pointer on the begin of the page.
 */
paddr_t pmm_alloc_page ()
{
    paddr_t page = find_free_page(PMM_LOWER_LIMIT);

    if (page & (PAGE_SIZE - 1)) {
        return pmm_alloc_page();
    }
    pmm_mark_page_as_used(page);
    return page;
}

/**
 * Reserve a page not under a entered address
 */
paddr_t pmm_alloc_page_limit(uint32_t lower_limit) {
    paddr_t page = find_free_page(PMM_LOWER_LIMIT > lower_limit ? PMM_LOWER_LIMIT : lower_limit);
    
    if(page & (PAGE_SIZE - 1)) {
        page = find_free_page(lower_limit);
    }

    if(page & (PAGE_SIZE - 1)) {
        panic("Can not reserve momory.\n");
    }
    pmm_mark_page_as_used(page);
    return page;
}


/**
 * Reserve num DMA-Pages.
 *
 * @return Pointer on the begin of the first page.
 */
paddr_t pmm_alloc_dma_page_range(unsigned int num) {
    paddr_t page = find_free_page_range(0, num);
    if((uint32_t) page & (PAGE_SIZE - 1)) {
        panic("There is no free momory now.");
    }
    pmm_mark_page_range_as_used(page, num);
    return page;
}

/**
 * Reserve num pages.
 *
 * @return Pointer on the begin of the first page.
 */

paddr_t pmm_alloc_page_range(unsigned int num) {
    paddr_t page = find_free_page_range(PMM_LOWER_LIMIT, num);
    if((uint32_t) page & (PAGE_SIZE - 1)) {
        return pmm_alloc_page_range(num);
    }
    pmm_mark_page_range_as_used(page, num);
    return page;
}

void INIT_PMM(struct multiboot_struct* MBS){
	unsigned long i;
	int x;
	struct mmap_entry* GRUB_MMAP =(struct mmap_entry*)MBS->mmap_addr;
	for(i=0;i<32768;i++){pmm_mmap[i]=0xFFFFFFFF;}

	while(i<=(MBS->mmap_length/24)){
		if(GRUB_MMAP[i].Type == 1){
			uint32_t base =(uint32_t) GRUB_MMAP[i].BaseAddr;
			uint32_t limit =(((uint32_t) GRUB_MMAP[i].Length)+base);
			while(base<limit){
				pmm_mark_page_as_used((paddr_t)base);
				base +=0x1000;
			}
		}
		i++;
	}

	//protect Memory structures
	pmm_mark_page_as_used(0); //IVT+BDA

	uint16_t* EBDA_p=(void*)0x040E;
	pmm_mark_page_as_used((paddr_t)EBDA_p[0]<<4); //EBDA

	uint16_t* BDA_size=0x0413;
	pmm_mark_page_as_used((BDA_size[0]/4)*1024); //FPS (maybe)

	pmm_mark_page_range_as_used(0xA0000,96); //0xA0000 - 0xFFFFF ROM-AREA

	//multiboot structures
	struct mods_add* MBMA=(void*)MBS->mods_addr;
	pmm_mark_page_as_used((paddr_t)MBS);
	pmm_mark_page_as_used((paddr_t)MBMA);

	//multiboot modules
	if(MBS->mods_count>0){
		for(x=0;x<MBS->mods_count;x++){
			uint32_t size=MBMA[x].mod_end-MBMA[x].mod_start;
			if(size%4096!=0){
				size/=4096;
				size++;
			}else{
				size/=4096;
			}
			pmm_mark_page_range_as_used((paddr_t)MBMA[x].mod_start,size);
			pmm_mark_page_as_used((paddr_t)MBMA[x].string);
		}
	}

	// Kernel
	pmm_mark_page_range_as_used((paddr_t)&kernel_start,((uint32_t)&kernel_end-(uint32_t)&kernel_start)/4096);
}

