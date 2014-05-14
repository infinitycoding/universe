#ifndef _PMM_H_
#define _PMM_H_

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
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <sys/multiboot.h>
#include <stdint.h>


extern const void kernel_start;
extern const void kernel_end;

// pages per byte
#define PAGES_PER_BYTE      8

// page size (4k)
#define PAGE_SIZE           4096

// page alignment
#define PAGE_ALIGN          PAGE_SIZE
#define PMM_MMAP_SIZE        32768

void INIT_PMM(struct multiboot_struct* MBS);
int pmm_count_free_pages(void);
int pmm_check_page(paddr_t page);
void pmm_mark_page_as_free(paddr_t page);
void pmm_mark_page_as_used(paddr_t page);
void pmm_mark_page_range_as_used(paddr_t page, unsigned int num);
void pmm_mark_page_range_as_free(paddr_t page, unsigned int num);
paddr_t pmm_find_free_page(unsigned long lower_limit);
paddr_t pmm_find_free_page_range(unsigned long lower_limit, unsigned int num);

paddr_t pmm_alloc_page(void);
paddr_t pmm_alloc_dma_page_range_64k(unsigned int num);
paddr_t pmm_alloc_page_limit(paddr_t lower_limit);
paddr_t pmm_alloc_page_range(unsigned int num);

#endif
