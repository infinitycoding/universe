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
	@author Michael Sippel (Universe Team) <micha.linuxfreak@gmail.com>
*/

#include <cpu.h>
#include <idt.h>
#include <paging.h>
#include <printf.h>
#include <pmm.h>
#include <drivers/video.h>
#include <memory_layout.h>
#include <string.h>
#include <panic.h>

static inline void paging_flush_tlb(vaddr_t addr);

#define PT_PADDR(i) (pd->entries[i] & ~0xFFF)
#define PT_VADDR(i) (MEMORY_LAYOUT_PAGING_STRUCTURES_START + PT_LENGTH*sizeof(pte_t)*i)

/**
 * Initalize paging (kernel_pd)
 *
 * @param void
 * @return void
 */
void ARCH_INIT_PAGING(struct multiboot_struct *mb_info) {
	vmm_context_t context;
	arch_context_t arch_context;

	context.arch_context = arch_context;

	paddr_t pframe = pmm_alloc_page_limit(0);
	vaddr_t vframe = pframe + MEMORY_LAYOUT_KERNEL_START;

	vaddr_t pt_vframe = MEMORY_LAYOUT_PAGING_STRUCTURES_START;

	arch_context.phys_addr = pframe;
	arch_context.entries = (pde_t *) vframe;
	arch_context.entries[PDE_INDEX(pt_vframe)] = pframe | VMM_WRITABLE | VMM_PRESENT;

	vmm_map_range(&context, 0, MEMORY_LAYOUT_KERNEL_START, NUM_PAGES(MEMORY_LAYOUT_DIRECT_MAPPED), VMM_WRITABLE);// kernel
	vmm_map(&context, 0xB8000, 0xC00B8000, VMM_WRITABLE | VMM_USER);// videomemory (0xB8000 - 0xBFFFF)
	// multiboot
	vmm_map(&context, ((vaddr_t)mb_info & (~0xfff)) - MEMORY_LAYOUT_KERNEL_START, ((paddr_t)mb_info&(~0xfff)), VMM_WRITABLE);
	vmm_map(&context, (mb_info->mods_addr & (~0xfff)) - MEMORY_LAYOUT_KERNEL_START, mb_info->mods_addr & (~0xfff), VMM_WRITABLE);

	int i;
	uintptr_t addr;
	struct mods_add *modules = (void*) mb_info->mods_addr;
	for(i = 0; i < mb_info->mods_count; i++) {
		addr = modules[i].mod_start & (~0xfff);
		while(addr < modules[i].mod_end) {
			vmm_map(&context, addr, addr, VMM_PRESENT | VMM_WRITABLE);
			addr += PAGE_SIZE;
		}
	}

	void *pd = (void *) vmm_automap_kernel(&context, pframe, VMM_WRITABLE);
	void *ct = (void *) vmm_automap_kernel(&context, pd_paddr, VMM_WRITABLE);
	arch_context.entries = pd;
	current_context = ct;
	asm volatile("mov %0, %%cr3" : : "r" (pframe));
}

/**
 * Create new pagedirectory
 *
 * @param void
 * @return new pagedirectory
 */
arch_vmm_context_t *arch_vmm_create_context(void) {
	uintptr_t paddr = (uintptr_t) pmm_alloc_page();
	arch_vmm_context_t *context = (arch_vmm_context_t *) pd_automap_kernel(current_context, paddr, VMM_PRESENT | VMM_WRITABLE);
	memset(context, 0, PAGE_SIZE);

	uintptr_t entries_paddr = (uintptr_t) pmm_alloc_page();
	uintptr_t entries = pd_automap_kernel(current_context, entries_paddr, VMM_PRESENT | VMM_WRITABLE);
	memset((void*)entries, 0, PAGE_SIZE);

	context->entries = (pde_t*) entries;
	context->phys_addr = entries_paddr;

	arch_update_context(context);
	context->entries[PDE_INDEX(MEMORY_LAYOUT_PAGING_STRUCTURES_START)] = (uint32_t) entries_paddr | VMM_PRESENT | VMM_WRITABLE;

	return context;
}

/**
 * Destroy a pagedirectory
 *

 * @param pd pagedirectory to destroy
 * @return void
 */
void arch_destroy_context(arch_vmm_context_t *context) {
	int pt;
	for (pt = 0; pt < PD_LENGTH; ++pt) {
		if (context->entries[pt] & VMM_PRESENT) {
			pmm_mark_page_as_free((paddr_t)context->entries[pt]);
		}
	}
	pmm_mark_page_as_free((paddr_t)context->phys_addr);
}

/**
 * Update the kernelspace area of a pagedir
 *
 * @param pagedir
 * @return void
 */
void arch_update_context(arch_vmm_context_t *context) {
	if(context != current_context) {
		#define START PDE_INDEX(MEMORY_LAYOUT_KERNEL_START)
		#define END   PDE_INDEX(MEMORY_LAYOUT_KERNEL_END)
		uintptr_t upd = (uintptr_t) (context->entries + START);
		uintptr_t kpd = (uintptr_t) (current_context->arch_context->entries + START);
		size_t len = END - START;
		memcpy((void*) upd, (void*) kpd, len * sizeof(pde_t));
		context->entries[PDE_INDEX(MEMORY_LAYOUT_PAGING_STRUCTURES_START)] = (uint32_t) context->phys_addr | VMM_PRESENT | VMM_WRITABLE;
	}
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

	if(pd_current != NULL) {
	  if(pd != pd_current) {
	    pt = (pt_t) PT_PADDR(index);
	    pt = (pt_t) vmm_automap_kernel(current_context,(paddr_t) pt, flags);
	  } else {
	    pt = (pt_t) PT_VADDR(index);
	  }
	} else {
	  pt = (pt_t) PT_PADDR(index) + MEMORY_LAYOUT_KERNEL_START;
	}

	return pt;
}

/**
 * Create a new pagetable
 *
 * @param pd pagedirectory
 * @param index index
 * @param flags flags
 *
 * @return pagetable
 */
pt_t pt_create(pd_t *pd, int index, uint8_t flags) {
	pt_t pt = (pt_t) pmm_alloc_page_limit(0);
	pd->entries[index] = (pde_t) pt | flags | VMM_PRESENT;

	pt = pt_get(pd, index, flags | VMM_PRESENT);
	memset(pt, 0, 4096);

	return pt;
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
	pmm_mark_page_as_free((paddr_t)pd->entries[index]);
	pd->entries[index] = 0;
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
int arch_map(arch_vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags) {
	if( (pframe & 0xFFF) || (vframe & 0xFFF) ) {
	  printf("pd_map(): can't map 0x%x to 0x%x.", pframe, vframe);
	  return -1;
	}

	uint32_t pd_index = PDE_INDEX(vframe);
	uint32_t pt_index = PTE_INDEX(vframe);

	pt_t pt = NULL;
	pde_t pde = context->entries[pd_index];

	if (pde & VMM_PRESENT) {
		pt = pt_get(pd, pd_index, flags | VMM_PRESENT);
	} else {
		pt = pt_create(pd, pd_index, flags | VMM_PRESENT);
	}

	pt[pt_index] = (pte_t)(pframe & ~0xFFF) | VMM_PRESENT | (flags & 0xFFF);

	if(context == current_context && current_context) {
		paging_flush_tlb(vframe);
	} else {
		if(current_context) {
			vmm_unmap(current_context, (vaddr_t) pt);
		}
	}

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
void arch_unmap(arch_vmm_context_t *context, vaddr_t frame) {
	pt_t pt = pt_get(context, PDE_INDEX(frame), VMM_WRITABLE);
	pt[PTE_INDEX(frame)] = 0;

	int pt_emty = 1, i;
	for(i = 0; i < PT_LENGTH; i++) {
	  if(pt[i]) {
	    pt_emty = 0;
	    break;
	  }
	}

	if(pt_emty) {
	  pt_destroy(context, PDE_INDEX(frame));
	}
}

/**
 * Find a free virtual adress
 *
 * @param pd pagedirectory
 * @return virtual adress
 */
vaddr_t vaddr_find(arch_vmm_context_t *context, int num, vaddr_t limit_low, vaddr_t limit_high, int flags) {
#define PAGES_FOUND(l) \
	  if(vaddr == (vaddr_t)NULL) { \
	    page = pd_index * PT_LENGTH + pt_index; \
	    vaddr = page * PAGE_SIZE; \
	  } \
	  pages_found += l; \
	  if(pages_found >= num) { \
	    return vaddr; \
	  }

  vaddr_t vaddr = 0;
  int page = 0;
  int pages_found = 0;

  uint32_t pd_index = PDE_INDEX(limit_low);
  uint32_t pt_index = PTE_INDEX(limit_low);
  uint32_t pd_index_end = PDE_INDEX(limit_high);
  uint32_t pt_index_end = PTE_INDEX(limit_high);
  pt_t pt;

  while(pd_index <= pd_index_end) {
    if(context->entries[pd_index] & VMM_PRESENT) {
      pt = pt_get(pd, pd_index, flags);

      uint32_t pt_end = (pd_index == pd_index_end) ? pt_index_end : PT_LENGTH; // last pd entry
      for(; pt_index < pt_end; pt_index++) {
	if(! ((uint32_t)pt[pt_index] & VMM_PRESENT) ) {
	  PAGES_FOUND(1);
	} else {
	  pages_found = 0;
	  vaddr = (uintptr_t)NULL;
	}
      }
      pt_index = 0;
    } else {
      PAGES_FOUND(PT_LENGTH);
    }
    pd_index++;
  }

  return 0;
}

paddr_t arch_vaddr2paddr(arch_vmm_context_t *context, vaddr_t vaddr) {
 	unsigned int pd_index = PDE_INDEX(vaddr);
 	unsigned int pt_index = PTE_INDEX(vaddr);

 	pt_t *pt = (pt_t *)pd_get(context->entries[pd_index] & PDE_FRAME, 0);
	return pt[pt_index];
}

/**
 * Switch the pagedirectory
 *
 * @param pd pagedirectory
 * @param flags flags
 *
 * @return void
 */
void arch_switch_context(arch_context_t *context) {
	asm volatile ("mov %0, %%cr3" : : "r" (context->phys_addr));
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

	uint32_t addr;
	asm ("mov %%cr2, %0" : "=r" (addr));

	sprintf(message, "Page fault in %s space:\nError %s address %#010X: %s.\nEIP: %#010X", ((cpu->error & 4) ? "user" : "kernel"),
		      ((cpu->error & 2) ? "writing to" : "reading at"), addr, ((cpu->error & 1) ? "Access denied" : "Nonpaged area"), cpu->eip);

	panic(message);
}

static inline void paging_flush_tlb(vaddr_t addr) {
	asm volatile ("invlpg %0" : : "m" (*(char*) addr));
}

