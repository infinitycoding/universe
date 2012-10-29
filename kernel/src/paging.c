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

#include <cpu.h>
#include <idt.h>
#include <paging.h>
#include <printf.h>
#include <pmm.h>
#include <drivers/video.h>
#include <memory_layout.h>
#include <boot_pd.h>

static inline void paging_flush_tlb(vaddr_t addr);

pd_t *pd_kernel = NULL;
pd_t *pd_current = NULL;
vaddr_t *temp_mapped = NULL;
#define TEMP_SIZE PAGE_SIZE
#define FOR_TEMP(x) \
    for(x = 0; x < TEMP_SIZE; x++)

/**
 * Initalize paging (kernel_pd)
 *
 * @param void
 * @return void
 */
void INIT_PAGING(void) {
	install_exc(INT_PAGE_FAULT, pd_fault_handler);

	pd_kernel = pmm_alloc_page();

	paddr_t pframe = pmm_alloc_page();
	vaddr_t vframe = MEMORY_LAYOUT_PAGING_STRUCTURES_START;

	pd_kernel->entries = pframe;
	pd_kernel->entries[PDE_INDEX(vframe)] = pframe | PTE_WRITABLE | PDE_PRESENT;
	pd_kernel->phys_addr = pframe;

	paddr_t temp_pframe = pmm_alloc_page();
	temp_mapped = pd_automap_kernel(pd_kernel, temp_pframe, PTE_WRITABLE);
	memset(pframe, 0, TEMP_SIZE);

	pd_map_range(
			pd_kernel,
			0x00000000,
			MEMORY_LAYOUT_KERNEL_START,
			MEMORY_LAYOUT_DIRECT_MAPPED / PAGE_SIZE,
			PTE_WRITABLE
	);

	pd_kernel->entries = (pde_t*) vframe;
	pd_switch(pd_kernel);
}

/**
 * Create new pagedirectory
 *
 * @param void
 * @return new pagedirectory
 */
pd_t *pd_create(void) {
	pd_t pd;

	paddr_t pframe = pmm_alloc_page();
	vaddr_t vframe = pd_map_temp(pframe, PTE_WRITABLE);
	memset(vframe, 0, PD_LENGTH);

	pd.entries = vframe;
	pd.phys_addr = pframe;

	return &pd;
}

/**
 * Destroy a pagedirectory
 *
 * @param pd pagedirectory to destroy
 * @return void
 */
void pd_destroy(pd_t *pd) {
	int pt;
	for (pt = 0; pt < PD_LENGTH; ++pt) {
		if (pd->entries[pt] & PDE_PRESENT) {
			pmm_mark_page_as_free((paddr_t)pd->entries[pt]);
		}
	}
	pmm_mark_page_as_free((paddr_t)pd->phys_addr);
}

/**
 * Get the pagetable at index
 *
 * @param pd pagedirectory
 * @param index index
 * @param flags flags
 *
 * @return pagetable
 */
pt_t pt_get(pd_t *pd, int index, uint8_t flags) {
	pt_t pt;

	if(pd_current) {
	  if(pd != pd_current) {
	    pt = (pt_t) (pd->entries[index] & ~0xFFF);
	    pt = pd_map_temp(pt, flags);
	  } else {
	    pt = (pt_t) (MEMORY_LAYOUT_PAGING_STRUCTURES_START + PT_LENGTH*4*index);
	  }
	} else {
	  pt = (pt_t) (pd->entries[index] & ~0xFFF);
	}

	return pt;
}

/**
 * Create a new pagetale
 *
 * @param pd pagedirectory
 * @param index index
 * @param flags flags
 *
 * @return pagetable
 */
pt_t pt_create(pd_t *pd, int index, uint8_t flags) {
	pt_t pt = pmm_alloc_page();
	memset(pt, 0, PT_LENGTH);

	pd->entries[index] = (int)pt | flags | PDE_PRESENT;

	return pt_get(pd, index, flags);
}

/**
 * Destroy a pagetable
 *
 * @param pd pagedirectory
 * @param index index
 *
 * @return void
 */
void pt_destroy(pd_t *pd, int index) {
	pd->entries[index] = 0;
}

/**
 * Map a page temporaily in the current pagedirectory
 *
 * @param pframe physical address
 * @param flags flags
 *
 * @return virtual address
 */
vaddr_t pd_map_temp(paddr_t pframe, uint8_t flags) {
	vaddr_t vframe = vaddr_find(pd_current,
				    MEMORY_LAYOUT_RESERVED_AREA_END,
				    MEMORY_LAYOUT_KERNEL_END);
	pd_map(pd_current, pframe, vframe, flags);

	int i;
	FOR_TEMP(i) {
	  if(temp_mapped[i] == NULL) {
	    /**
	     * FIXME: if temp_mapped is full the mapping won't be removed at pd-switch!
	     */
	    temp_mapped[i] = vframe;
	    break;
	  }
	}

	return vframe;
}

/**
 * Map a physical address to a virtual adress
 *
 * @param pd pagedirectory in that will map
 * @param pframe physical adress
 * @param vframe vitual adress
 * @param flags flags
 *
 * @return success
 */
int pd_map(pd_t *pd, paddr_t pframe, vaddr_t vframe, uint8_t flags) {
	if( (pframe & 0xFFF) || (vframe & 0xFFF) ) {
	  printf("pd_map(): can't map 0x%x to 0x%x.", pframe, vframe);
	  return -1;
	}

	uint32_t pd_index = PDE_INDEX(vframe);
	uint32_t pt_index = PTE_INDEX(vframe);

	pt_t pt = NULL;
	pde_t pde = pd->entries[pd_index];

	if (pde & PDE_PRESENT) {
		pt = pt_get(pd, pd_index, flags);
	} else {
		pt = pt_create(pd, pd_index, flags);
	}

	pt[pt_index] = (uint32_t)(pframe & PTE_FRAME) | PTE_PRESENT | (flags & 0xFFF);
	paging_flush_tlb(vframe);

	return 0;
}

/**
 * Remove a mapping from pagedir/pagetable
 *
 * @param pd pagedirectory
 * @param frame adress to unmap
 *
 * @return void
 */
void pd_unmap(pd_t *pd, vaddr_t frame) {
	pt_t pt = pt_get(pd, PDE_INDEX(frame), PDE_WRITABLE);
	pt[PTE_INDEX(frame)] = 0;

	int pt_emty = 1, i;
	for(i = 0; i < PT_LENGTH; i++) {
	  if(pt[i]) {
	    pt_emty = 0;
	    break;
	  }
	}

	if(pt_emty) {
	  pt_destroy(pd, PDE_INDEX(frame));
	}
}

void pd_map_range(pd_t *pd, paddr_t pframe, vaddr_t vframe, unsigned int pages, uint8_t flags) {
	int p;
	for (p = 0; p < pages; ++p) {
	  pd_map(pd, pframe + PAGE_FRAME_ADDR(p), vframe + PAGE_FRAME_ADDR(p), flags);
	}
}

void pd_unmap_range(pd_t *pd, vaddr_t frame, unsigned int pages) {
	int p;
	for(p = 0; p < pages; p++) {
	  pd_unmap(pd, frame + PAGE_FRAME_ADDR(p));
	}
}

/**
 * Auto-map pframe to a free virtual address in kernelspace
 *
 * @param pd pagedirectory
 * @param pframe phys. address
 * @param flags flags
 *
 * @return virtual address
 */
vaddr_t pd_automap_kernel(pd_t *pd, paddr_t pframe, uint8_t flags) {
  vaddr_t vframe = vaddr_find(pd,
			      MEMORY_LAYOUT_RESERVED_AREA_END,
			      MEMORY_LAYOUT_KERNEL_END);
  pd_map(pd, pframe, vframe, flags);

  return vframe;
}

/**
 * Auto-map pframe to a free virtual address in userspace
 *
 * @param pd pagedirectory
 * @param pframe phys. address
 * @param flags flags
 *
 * @return virtual address
 */
vaddr_t pd_automap_user(pd_t *pd, paddr_t pframe, uint8_t flags) {
  vaddr_t vframe = vaddr_find(pd,
			      0x00000000,
			      MEMORY_LAYOUT_KERNEL_START);
  pd_map(pd, pframe, vframe, flags);

  return vframe;
}

/**
 * Find a free virtual adress
 *
 * @param pd pagedirectory
 * @return virtual adress
 */
vaddr_t vaddr_find(pd_t *pd, int limit_low, int limit_high) {
	vaddr_t vaddr = NULL;
	pt_t *pt;
	uint32_t pd_index = PDE_INDEX(limit_low);
	uint32_t pt_index = PTE_INDEX(limit_low);
	uint32_t pd_end = PDE_INDEX(limit_high);
	uint32_t pt_end = PTE_INDEX(limit_high);

	while(pd_index <= pd_end) {
	  if(pd->entries[pd_index] & PDE_PRESENT) {
	    pt = pt_get(pd, pd_index, PTE_WRITABLE);
	    uint32_t end = (pd_index == pd_end) ? pt_end : PT_LENGTH;
	    while(pt_index < end) {
		if(! ((int)pt[pt_index++] & PTE_PRESENT) ) {
		  vaddr = PAGE_FRAME_ADDR( pd_index*PT_LENGTH + pt_index );
		  return vaddr;
		}
	    }
	    pt_index = 0;
	  } else {
	    vaddr = PAGE_FRAME_ADDR( pd_index*PT_LENGTH + pt_index );
	    return vaddr;
	  }
	  pd_index++;
	}

	return NULL;
}

/** FIXME: really needed? */
paddr_t vaddr2paddr(pd_t * const pd, vaddr_t vaddr)
{
// 	unsigned int pd_index = PDE_INDEX(vaddr);
// 	unsigned int pt_index = PTE_INDEX(vaddr);
//
// 	pt_t *pt = (pt_t *)pd_map_fast(pd->entries[pd_index] & PDE_FRAME, 0);
//
// 	return (paddr_t)(pd_map_fast(pt[pt_index] & PTE_FRAME, 0) + (vaddr & 0xFFF));
	return NULL;
}

/**
 * Switch the pagedirectory
 *
 * @param pd pagedirectory
 * @param flags flags
 *
 * @return void
 */
void pd_switch(pd_t *pd) {
    if(pd != pd_current) {
	if(pd_current != NULL) {
	  pd_unmap_range(pd_current, temp_mapped, TEMP_SIZE);
	}

	pd_current = pd;
	asm volatile ("mov %0, %%cr3" : : "r" (pd->phys_addr));
    }
}

/**
 * Exception handler for pagefaults
 *
 * @param cpu current cpu state
 * @return void
 */
void pd_fault_handler(struct cpu_state **cpu_p)
{
    struct cpu_state *cpu = cpu_p[0];
	char message[512];
	int len = 0;

	uint32_t addr;
	asm ("mov %%cr2, %0" : "=r" (addr));

	len = sprintf(message, "Page fault in %s space:\nError %s address %#010X: %s.\nEIP: %#010X", ((cpu->error & 4) ? "user" : "kernel"),
		      ((cpu->error & 2) ? "writing to" : "reading at"), addr, ((cpu->error & 1) ? "Access denied" : "Nonpaged area"), cpu->eip);

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

static inline void paging_flush_tlb(vaddr_t addr) {
	asm volatile ("invlpg (%0)" : : "r" (addr) : "memory");
}
