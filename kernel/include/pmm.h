#ifndef _pmm_h_
#define _pmm_h_
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


	#include <stdint.h>
	#include <multiboot.h>

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
	void pmm_mark_page_as_free(paddr_t page);
	unsigned long pmm_count_free_pages(void);

	paddr_t pmm_alloc_page ();
	paddr_t pmm_alloc_dma_page_range_64k(unsigned int num);
	paddr_t pmm_alloc_page_limit(uint32_t lower_limit);
	paddr_t pmm_alloc_page_range(unsigned int num);

#endif
