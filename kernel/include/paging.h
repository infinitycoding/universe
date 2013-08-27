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
*/

/**
	@author Tom Slawik <tom.slawik@gmail.com>
	@author Michael Sippel (Universe Team) <micha.linuxfreak@gmail.com>
*/

#include <stdint.h>

enum CR3_Flags {
	CR3_WRITETHOUGH	= 8,
	CR3_NOCACHE	= 16,
	CR3_FRAME	= 0x7FFFF000
};

enum PDE_Flags {
	PDE_PRESENT	= 1,
	PDE_WRITABLE	= 2,
	PDE_USER	= 4,
	PDE_WRITETHOUGH	= 8,
	PDE_NOCACHE	= 16,
	PDE_ACCESSED	= 32,
	PDE_DIRTY	= 64,
	PDE_4MB		= 128,
	PDE_GLOBAL	= 256,
	PDE_AVAIL	= 0xE00,
	PDE_FRAME	= 0x7FFFF000,
};

enum PTE_Flags {
	PTE_PRESENT	= 1,
	PTE_WRITABLE	= 2,
	PTE_USER	= 4,
	PTE_WRITETHOUGH	= 8,
	PTE_NOCACHE	= 16,
	PTE_ACCESSED	= 32,
	PTE_DIRTY	= 64,
	PTE_PAT		= 128,
	PTE_GLOBAL	= 256,
	PTE_AVAIL	= 0xE00,
	PTE_FRAME	= 0x7FFFF000
};

#define PD_LENGTH 1024
#define PT_LENGTH 1024

#define PAGE_FRAME_NUMBER(x) ((x) >> 12)
#define PAGE_FRAME_ADDR(x) ((x) << 12)

#define PDE_INDEX(x) ((x) >> 22)
#define PTE_INDEX(x) ((x) >> 12 & 0x03FF)

typedef uint32_t pde_t;
typedef uint32_t pte_t;
typedef pte_t* pt_t;

typedef struct {
	pde_t *entries;
	paddr_t phys_addr;
} pd_t;

void INIT_PAGING(void);

pd_t *pd_create(void);
void pd_destroy(pd_t *pd);

vaddr_t pd_map_temp(paddr_t pframe, uint8_t flags);
int pd_map(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags);
void pd_unmap(pd_t *pd, vaddr_t frame);
void pd_map_range(pd_t *pd, paddr_t pframe, vaddr_t vframe, unsigned int pages, uint8_t flags);
void pd_unmap_range(pd_t *pd, vaddr_t frame, unsigned int pages);
vaddr_t pd_automap_kernel(pd_t *pd, paddr_t pframe, uint8_t flags);
vaddr_t pd_automap_user(pd_t *pd, paddr_t pframe, uint8_t flags);

pt_t pt_get(pd_t *pd, int index, uint8_t flags);
pt_t pt_create(pd_t *pd, int index, uint8_t flags);
void pt_destroy(pd_t *pd, int index);
void pd_update(pd_t *pd);

vaddr_t vaddr_find(pd_t *pd, int num, vaddr_t limit_low, vaddr_t limit_high, int flags);
paddr_t vaddr2paddr(pd_t * const pd, vaddr_t vaddr); // FIXME: really needed?

void pd_switch(pd_t *pd);

//void map_page_kernel(paddr_t phys_frame, vaddr_t virt_frame, uint8_t flags);
//void unmap_page_kernel(vaddr_t frame);

void pd_fault_handler(struct cpu_state **cpu_p);

inline pd_t * pd_get_current(void);
inline pd_t * pd_get_kernel(void);

#endif
