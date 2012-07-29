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

#include <paging.h>
#include <pmm.h>

pd_t *pd_kernel;
pd_t *pd_current;

void INIT_PAGING(void)
{
	pd_kernel = pd_create();
	pd_switch(pd_kernel, 0);
	pd_map_range(pd_kernel, 0x00000000, 0x00000000, PTE_WRITABLE, 1024);
 	pd_map_range(pd_kernel, &kernel_start, 0xC0000000, PTE_WRITABLE, PAGE_INDEX(&kernel_end - &kernel_start));
	pd_enable_paging();
}

pd_t *pd_create(void)
{
	pd_t *pd = (pd_t *)pmm_alloc_page();
	memset(pd, 0, sizeof(pd_t));
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

void pd_map(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags)
{
	pt_t *pt = NULL;
	
	if (pd->entries[PDE_INDEX(vframe)] & PDE_PRESENT) {
		pt = (pt_t *)((uint32_t)pd->entries[PDE_INDEX(vframe)] & PDE_FRAME);
	} else {
		pt = (pt_t *)pmm_alloc_page();
		memset(pt, 0, sizeof(pt_t));
		pd->entries[PDE_INDEX(vframe)] = (uint32_t)pt | PDE_PRESENT;
	}
	
	pt->entries[PTE_INDEX(vframe)] = (uint32_t)pframe | PTE_PRESENT | (flags & 0xFFF);
	
	pd_flush_tlb(vframe);
}

void pd_unmap(pd_t *pd, vaddr_t frame)
{
	pt_t *pt = (pt_t *)((uint32_t)pd->entries[PDE_INDEX(frame)] & PDE_FRAME);
	pt->entries[PTE_INDEX(frame)] = 0;
	pd_flush_tlb(frame);
}

void pd_map_range(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags, unsigned int pages)
{
	int p;
	for (p = 0; p < pages; ++p) {
		pd_map(pd, pframe + PAGE_ADDR(p), vframe + PAGE_ADDR(p), flags);
	}
}

vaddr_t pd_map_fast(paddr_t frame, uint8_t flags)
{
	int t, e;
	
	for (t = 0; t < PD_LENGTH; ++t)
	{
		vaddr_t vframe;
		
		if (pd_current->entries[t] & PDE_PRESENT)
		{
			pt_t *pt = (pt_t *)(pd_current->entries[t] & PDE_FRAME);
			
			for (e = 0; e < PT_LENGTH; ++e)
			{
				if (!(pt->entries[e] & PTE_PRESENT))
				{
					vframe = (t << 22) + (e << 12);
					pd_map(pd_current, frame, vframe, flags);
					return vframe;
				}
			}
		} else {
			vframe = (t << 22);
			pd_map(pd_current, frame, vframe, flags);
			return vframe;
		}
	}
	
	/* too less memory */
	return -1;
}

void pd_switch(pd_t *pd, uint8_t flags)
{
	pd_current = pd;
	uint32_t descriptor = ((paddr_t)pd & PD_FRAME) | flags;
	asm volatile ("mov %0, %%cr3" : : "r" (descriptor));
}

static inline void pd_enable_paging(void)
{
	asm volatile (
		"mov %cr0, %eax;"
		"or $0x80000000, %eax;"
		"mov %eax, %cr0"
	);
}

static inline void pd_flush_tlb(vaddr_t addr)
{
	asm volatile("invlpg (%0)" : : "r" (addr) : "memory");
}
