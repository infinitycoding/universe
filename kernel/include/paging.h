#ifndef _paging_h_
#define _paging_h_
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

    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

/**
	@author Tom Slawik <tom.slawik@gmail.com>
*/

#include <stdint.h>

#define PD_WT			8
#define PD_NOCACHE		16
#define PD_FRAME		0x7FFFF000

#define PDE_PRESENT		1
#define PDE_WRITABLE		2
#define PDE_USER		4
#define PDE_WRITETHOUGH		8
#define PDE_NOCACHE		16
#define PDE_ACCESSED		32
#define PDE_DIRTY		64
#define PDE_4MB			128
#define PDE_GLOBAL		256
#define PDE_AVAIL		0xE00
#define PDE_FRAME		0x7FFFF000

#define PTE_PRESENT		1
#define PTE_WRITABLE		2
#define PTE_USER		4
#define PTE_WRITETHOUGH		8
#define PTE_NOCACHE		16
#define PTE_ACCESSED		32
#define PTE_DIRTY		64
#define PTE_GLOBAL		256
#define PTE_AVAIL		0xE00
#define PTE_FRAME		0x7FFFF000

#define PD_LENGTH 	1024
#define PT_LENGTH 	1024

#define PAGE_ADDR(x) ((x) << 12)
#define PAGE_INDEX(x) ((x) >> 12)

#define PDE_INDEX(x) ((x) >> 22)
#define PTE_INDEX(x) ((x) >> 12 & 0x03FF)

typedef uint32_t pde_t;

typedef uint32_t pte_t;

typedef struct {
	pde_t entries[PD_LENGTH];
} pd_t;

typedef struct {
	pte_t entries[PT_LENGTH];
} pt_t;

void INIT_PAGING(void);

pd_t *pd_create(void);
void pd_destroy(pd_t *pd);

void pd_map(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags);
void pd_unmap(pd_t *pd, vaddr_t frame);
void pd_map_range(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags, unsigned int pages);
vaddr_t pd_map_fast(paddr_t frame, uint8_t flags);

void pd_switch(pd_t *pd, uint8_t flags);
static inline void pd_enable_paging(void);
static inline void pd_flush_tlb(vaddr_t addr);

//void map_page_kernel(paddr_t phys_frame, vaddr_t virt_frame, uint8_t flags);
//void unmap_page_kernel(vaddr_t frame);

#endif
