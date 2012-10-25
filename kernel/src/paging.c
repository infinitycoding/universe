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
*/

#include <cpu.h>
#include <idt.h>
#include <paging.h>
#include <pmm.h>
#include <drivers/video.h>
#include <memory_layout.h>
#include <boot_pd.h>

static inline void paging_enable(void);
static inline void paging_disable(void);
static inline void paging_flush_tlb(vaddr_t addr);

volatile pd_t pd_fast __attribute__ ((aligned (PAGE_SIZE)));
volatile pt_t pt_fast __attribute__ ((aligned (PAGE_SIZE)));

pd_t *pd_kernel;
pd_t *pd_current;

void INIT_PAGING(void)
{
	install_exc(INT_PAGE_FAULT, pd_fault_handler);
	
	pd_map(
			&pd_fast,
			&pt_fast,
			MEMORY_LAYOUT_PAGING_STRUCTURES_START,
			PTE_WRITABLE
	);
	
	pd_kernel = &pd_fast;
	
	pd_map_range(
			pd_kernel,
			0x00000000,
			MEMORY_LAYOUT_KERNEL_START,
			MEMORY_LAYOUT_DIRECT_MAPPED / PAGE_SIZE,
			PTE_WRITABLE
	);
	
	pd_switch(pd_kernel, 0);
}

pd_t *pd_create(void)
{
	pd_t *pd;
	paddr_t pframe = 0x1990000; /* WARNING: Random memory address, not protected. */
	vaddr_t vframe = pd_map_fast(pframe, 0);
	
	pd = (pd_t *)vframe;
	pd->phys_addr = pframe; /* WARNING: Overflow */
	
	paging_flush_tlb(vframe);
	
	return pd;
}

void pd_destroy(pd_t *pd)
{
	int pt;
	for (pt = 0; pt < PD_LENGTH; ++pt) {
		if (pd->entries[pt] & PDE_PRESENT) {
			pmm_mark_page_as_free((paddr_t)pd->entries[pt]);
		}
	}
	pmm_mark_page_as_free((paddr_t)pd);
}

void pd_map(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags) /* FIXME: Differentiate PDE flags and PTE flags */
{
	uint32_t pd_index = PDE_INDEX(vframe);
	uint32_t pt_index = PTE_INDEX(vframe);
	
	pt_t *pt = NULL;

	if (pd->entries[pd_index] & PDE_PRESENT) {
		pt = (pt_t *)pd_map_fast(((uint32_t)pd->entries[PDE_INDEX(vframe)] & PDE_FRAME), 0);
	} else { /* set up new page table */
		pt = (pt_t *)pmm_alloc_page();
		memset(pt, 0, sizeof(pt_t));
		pd->entries[pd_index] = (uint32_t)pt | PDE_PRESENT;
	}

	pt->entries[pt_index] = (uint32_t)(pframe & PTE_FRAME) | PTE_PRESENT | (flags & 0xFFF);

	paging_flush_tlb(vframe);
}

void pd_unmap(pd_t *pd, vaddr_t frame)
{
	pt_t *pt = (pt_t *)((uint32_t)pd->entries[PDE_INDEX(frame)] & PDE_FRAME);
	pt->entries[PTE_INDEX(frame)] = 0;
	paging_flush_tlb(frame);
}

void pd_map_range(pd_t *pd, paddr_t pframe, vaddr_t vframe, unsigned int pages, uint8_t flags)
{
	int p;
	for (p = 0; p < pages; ++p) {
		pd_map(pd, pframe + PAGE_FRAME_ADDR(p), vframe + PAGE_FRAME_ADDR(p), flags);
	}
}

vaddr_t pd_map_fast(paddr_t frame, uint32_t flags)
{
	if (frame < MEMORY_LAYOUT_DIRECT_MAPPED) {
		return MEMORY_LAYOUT_KERNEL_START + frame;
	} else {
		int i;
		for (i = 0; i < 1024; ++i) {
			if (!(pt_fast.entries[i] & PDE_PRESENT)) {
				pt_fast.entries[i] = (uint32_t)(frame & PTE_FRAME) | PTE_PRESENT | (flags & 0xFFF);
				
				return MEMORY_LAYOUT_PAGING_STRUCTURES_START + (frame & 0xFFF) + i * PAGE_SIZE;
			}
		}
		return 0; /* mapping not possible */		
	}
}

paddr_t vaddr2paddr(pd_t * const pd, vaddr_t vaddr)
{
	unsigned int pd_index = PDE_INDEX(vaddr);
	unsigned int pt_index = PTE_INDEX(vaddr);
	
	pt_t *pt = (pt_t *)pd_map_fast(pd->entries[pd_index] & PDE_FRAME, 0);
	
	return (paddr_t)(pd_map_fast(pt->entries[pt_index] & PTE_FRAME, 0) + (vaddr & 0xFFF));
}

void pd_switch(pd_t *pd, uint8_t flags)
{
	//paddr_t frame = vaddr2paddr(pd, (vaddr_t)pd);
	paddr_t frame = (uintptr_t)pd - MEMORY_LAYOUT_KERNEL_START;
	//printf("[PAGING] Switching to pagedir %p\n", frame);
	
	pd_current = pd;
	uint32_t descriptor = (frame & CR3_FRAME) | flags;
	asm volatile ("mov %0, %%cr3" : : "r" (descriptor));
}

void pd_fault_handler(struct cpu_state *cpu)
{
	char message[512];
	int len = 0;
	
	uint32_t addr;
	asm ("mov %%cr2, %0" : "=r" (addr));
	
	len = sprintf(message, "Page fault in %s space:\nError %s address %#010X: %s.\n", ((cpu->error & 4) ? "user" : "kernel"),
		      ((cpu->error & 2) ? "writing to" : "reading at"), addr, ((cpu->error & 1) ? "Access denied" : "Nonpaged area"));
	
	panic(message);
}

inline pd_t * pd_get_current(void)
{
	return pd_current;
}

inline pd_t * pd_get_kernel(void)
{
	return pd_kernel;
}

static inline void paging_flush_tlb(vaddr_t addr)
{
	asm volatile (
		"invlpg (%0)" : : "r" (addr) : "memory"
	);
}

static inline void paging_enable(void)
{
	asm volatile (
		"mov %cr0, %eax;"
		"or $0x80000000, %eax;"
		"mov %eax, %cr0"
	);
}

static inline void paging_disable(void)
{
	asm volatile (
		"mov %cr0, %eax;"
		"and $0x7FFFFFFF, %eax;"
		"mov %eax, %cr0"
	);
}
